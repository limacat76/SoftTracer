#pragma once
class CallableAlgorithm {
public:
	CallableAlgorithm() {
	}

	virtual ~CallableAlgorithm() {
	}

	virtual void initialize_scene() = 0;

	/**
	 * The render() function willfully does not specify neither the type of target, nor mailbox nor parameters,
	 * to allow implementors to decide on classes they need.
	 */
	virtual void render(void* target, void* mailbox, const void* parameters) = 0;

};

