#pragma once
//class WorkEngine {
//public:
//	WorkEngine() {
//	}
//
//	virtual ~WorkEngine() {
//	}
//
//	virtual void initialize_scene(const void* parameters) = 0;
//
//	/**
//	 * The render() function willfully does not specify neither the type of target, nor mailbox nor parameters,
//	 * to allow implementors to decide on classes they need.
//	 */
//	virtual void render(void* target, void* mailbox, const void* parameters) = 0;
//
//};

class WorkEngine {
public:
	WorkEngine() {
	}

	virtual ~WorkEngine() {
	}

	virtual void initialize_scene(const void* parameters) = 0;

	/**
	* The job() function willfully does not specify neither the type of target, nor mailbox nor parameters,
	* to allow implementors to decide on classes they need.
	*/
	virtual void job(void* target, void* mailbox, const void* parameters) = 0;

};
