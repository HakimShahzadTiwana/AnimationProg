#include "GltfAnimationChannel.h"

void GltfAnimationChannel::loadChannelData(std::shared_ptr<tinygltf::Model> model, tinygltf::Animation anim, tinygltf::AnimationChannel channel)
{
	mTargetNode = channel.target_node;

	// Get Accesser for input sampler from animation object, index stored in channel object
	const tinygltf::Accessor& inputAccessor = model->accessors.at(anim.samplers.at(channel.sampler).input);
	const tinygltf::BufferView& inputBufferView = model->bufferViews.at(inputAccessor.bufferView);
	const tinygltf::Buffer& inputBuffer = model->buffers.at(inputBufferView.buffer);

	// Get timings
	std::vector<float> timings;
	timings.resize(inputAccessor.count);
	std::memcpy(timings.data(), &inputBuffer.data.at(0) + inputBufferView.byteOffset, inputBufferView.byteLength);
	setTimings(timings);

	// Get sampler object
	const tinygltf::AnimationSampler sampler = anim.samplers.at(channel.sampler);

	// Get interpolation type from sampler 
	if (sampler.interpolation.compare("STEP") == 0) 
	{
		mInterType = EInterpolationType::STEP;
	}
	else if (sampler.interpolation.compare("LINEAR") == 0)
	{
		mInterType = EInterpolationType::LINEAR;
	}
	else 
	{
		mInterType = EInterpolationType::CUBICSPLINE;
	}


	// Get Accesser for oupput sampler from animation object, index stored in channel object
	const tinygltf::Accessor& outputAccessor = model->accessors.at(anim.samplers.at(channel.sampler).output);
	const tinygltf::BufferView& outputBufferView = model->bufferViews.at(outputAccessor.bufferView);
	const tinygltf::Buffer& outputBuffer = model->buffers.at(outputBufferView.buffer);

	// Get target type
	if (channel.target_path.compare("rotation") == 0) 
	{
		mTargetPath = ETargetPath::ROTATION;
		std::vector<glm::quat> rotations;
		rotations.resize(outputAccessor.count);
		std::memcpy(rotations.data(), &outputBuffer.data.at(0) + outputBufferView.byteOffset, outputBufferView.byteLength);
		setRotations(rotations);
	}
	else if (channel.target_path.compare("translation") == 0)
	{
		mTargetPath = ETargetPath::TRANSLATION;
		std::vector<glm::vec3> translations;
		translations.resize(outputAccessor.count);
		std::memcpy(translations.data(), &outputBuffer.data.at(0) + outputBufferView.byteOffset, outputBufferView.byteLength);
		setTranslations(translations);
	}
	else
	{
		mTargetPath = ETargetPath::SCALE;
		std::vector<glm::vec3> scales;
		scales.resize(outputAccessor.count);
		std::memcpy(scales.data(), &outputBuffer.data.at(0) + outputBufferView.byteOffset, outputBufferView.byteLength);
		setScalings(scales);
	}

}

void GltfAnimationChannel::setTimings(std::vector<float> timinings) {
	mTimings = timinings;
}

void GltfAnimationChannel::setScalings(std::vector<glm::vec3> scalings) {
	mScaling = scalings;
}

void GltfAnimationChannel::setTranslations(std::vector<glm::vec3> tranlations) {
	mTranslations = tranlations;
}

void GltfAnimationChannel::setRotations(std::vector<glm::quat> rotations) {
	mRotations = rotations;
}

int GltfAnimationChannel::getTargetNode() {
	return mTargetNode;
}

ETargetPath GltfAnimationChannel::getTargetPath() {
	return mTargetPath;
}

glm::vec3 GltfAnimationChannel::getTranslation(float time) {
	if (mTranslations.size() == 0)
	{
		return glm::vec3(1.0f);
	}
	if (time < mTimings.at(0))
	{
		return mTranslations.at(0);
	}
	if (time > mTimings.at(mTimings.size() - 1))
	{
		return mTranslations.at(mTranslations.size() - 1);
	}

	int prevTimeIndex = 0;
	int nextTimeIndex = 0;
	for (int i = 0; i < mTimings.size(); ++i)
	{
		if (mTimings.at(i) > time)
		{
			nextTimeIndex = i;
			break;
		}
		prevTimeIndex = i;
	}

	if (prevTimeIndex == nextTimeIndex)
	{
		return mTranslations.at(prevTimeIndex);
	}

	glm::vec3 finalTranslation = glm::vec3(1.0f);

	switch (mInterType)
	{
	case EInterpolationType::STEP:
		finalTranslation = mTranslations.at(prevTimeIndex);
		break;

	case EInterpolationType::LINEAR:
	{
		float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));

		glm::vec3 prevTranslation = mTranslations.at(prevTimeIndex);
		glm::vec3 nextTranslation = mTranslations.at(nextTimeIndex);
		finalTranslation = prevTranslation + interpolatedTime * (nextTranslation - prevTranslation);
	}
	break;

	case EInterpolationType::CUBICSPLINE:
	{
		// Values stored as in-tangent, data value, out-tangent for each time entry
		float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);

		// Tangents are normalized, so we need to scale it according to deltaTime
		glm::vec3 prevTangent = deltaTime * mTranslations.at(prevTimeIndex * 3 + 2);
		glm::vec3 nextTangent = deltaTime * mTranslations.at(nextTimeIndex * 3);


		// Get final scale by using the hermite formula
		float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
		float interpolatedTimeSq = interpolatedTime * interpolatedTime;
		float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;
		glm::vec3 prevPoint = mTranslations.at(prevTimeIndex * 3 + 1);
		glm::vec3 nextPoint = mTranslations.at(nextTimeIndex * 3 + 1);


		finalTranslation = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint + (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) * prevTangent + (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint + (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
	}
	break;
	}

	return finalTranslation;
}

glm::vec3 GltfAnimationChannel::getScaling(float time) {
	if (mScaling.size() == 0)
	{
		return glm::vec3(1.0f);
	}
	if (time < mTimings.at(0))
	{
		return mScaling.at(0);
	}
	if (time > mTimings.at(mTimings.size() - 1))
	{
		return mScaling.at(mScaling.size() - 1);
	}

	int prevTimeIndex = 0;
	int nextTimeIndex = 0;
	for (int i = 0; i < mTimings.size(); ++i)
	{
		if (mTimings.at(i) > time)
		{
			nextTimeIndex = i;
			break;
		}
		prevTimeIndex = i;
	}

	if (prevTimeIndex == nextTimeIndex)
	{
		return mScaling.at(prevTimeIndex);
	}

	glm::vec3 finalScale = glm::vec3(1.0f);

	switch (mInterType)
	{
		case EInterpolationType::STEP:
			finalScale = mScaling.at(prevTimeIndex);
			break;

		case EInterpolationType::LINEAR:
		{
			float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));

			glm::vec3 prevScale = mScaling.at(prevTimeIndex);
			glm::vec3 nextScale = mScaling.at(nextTimeIndex);
			finalScale = prevScale + interpolatedTime * (nextScale - prevScale);
		}
		break;

		case EInterpolationType::CUBICSPLINE:
		{
			// Values stored as in-tangent, data value, out-tangent for each time entry
			float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);

			// Tangents are normalized, so we need to scale it according to deltaTime
			glm::vec3 prevTangent = deltaTime * mScaling.at(prevTimeIndex * 3 + 2);
			glm::vec3 nextTangent = deltaTime * mScaling.at(nextTimeIndex * 3);


			// Get final scale by using the hermite formula
			float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
			float interpolatedTimeSq = interpolatedTime * interpolatedTime;
			float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;
			glm::vec3 prevPoint = mScaling.at(prevTimeIndex * 3 + 1);
			glm::vec3 nextPoint = mScaling.at(nextTimeIndex * 3 + 1);


			finalScale = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint + (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) * prevTangent + (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint + (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
		}
			break;
	}

	return finalScale;
}

glm::quat GltfAnimationChannel::getRotation(float time) {
	if (mRotations.size() == 0)
	{
		return glm::identity<glm::quat>();
	}
	if (time < mTimings.at(0))
	{
		return mRotations.at(0);
	}
	if (time > mTimings.at(mTimings.size() - 1))
	{
		return mRotations.at(mRotations.size() - 1);
	}

	int prevTimeIndex = 0;
	int nextTimeIndex = 0;
	for (int i = 0; i < mTimings.size(); ++i)
	{
		if (mTimings.at(i) > time)
		{
			nextTimeIndex = i;
			break;
		}
		prevTimeIndex = i;
	}

	if (prevTimeIndex == nextTimeIndex)
	{
		return mRotations.at(prevTimeIndex);
	}

	glm::quat finalRotation = glm::identity<glm::quat>();

	switch (mInterType)
	{
	case EInterpolationType::STEP:
		finalRotation = mRotations.at(prevTimeIndex);
		break;

	case EInterpolationType::LINEAR:
	{
		float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));

		glm::quat prevRotation = mRotations.at(prevTimeIndex);
		glm::quat nextRotation = mRotations.at(nextTimeIndex);
		finalRotation = prevRotation + interpolatedTime * (nextRotation - prevRotation);
	}
	break;

	case EInterpolationType::CUBICSPLINE:
	{
		// Values stored as in-tangent, data value, out-tangent for each time entry
		float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);

		// Tangents are normalized, so we need to scale it according to deltaTime
		glm::quat prevTangent = deltaTime * mRotations.at(prevTimeIndex * 3 + 2);
		glm::quat nextTangent = deltaTime * mRotations.at(nextTimeIndex * 3);


		// Get final scale by using the hermite formula
		float interpolatedTime = (time - mTimings.at(prevTimeIndex)) / (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
		float interpolatedTimeSq = interpolatedTime * interpolatedTime;
		float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;
		glm::quat prevPoint = mRotations.at(prevTimeIndex * 3 + 1);
		glm::quat nextPoint = mRotations.at(nextTimeIndex * 3 + 1);


		finalRotation = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint + (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) * prevTangent + (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint + (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
	}
	break;
	}

	return finalRotation;
}

float GltfAnimationChannel::getMaxTime()
{
	return mTimings.at(mTimings.size() - 1);
}