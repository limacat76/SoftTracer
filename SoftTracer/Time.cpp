#include<string>
#include <ctime>

namespace limacat {
	std::string take_my_time() {
		struct tm newtime;
		__time32_t aclock;

		char buffer[32];
		errno_t errNum;
		_time32(&aclock);   // Get time in seconds.  
		_localtime32_s(&newtime, &aclock);   // Convert time to struct tm form.  
											 // Print local time as a string.  

		errNum = asctime_s(buffer, 32, &newtime);
		if (errNum)
		{
			return "Error while getting the time";
		}
		return buffer;
	}
}