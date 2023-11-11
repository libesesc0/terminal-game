/*
 * I have not written this code and I do not know from where
 * is it from, a friend of mine gave it to me.
 *
 * If you have more information about it, contact me.
 */

#include <sys/poll.h>		// poll pollfd
#include <string>
#include <vector>
#include <linux/input.h>		// input_event, EVIOCBIT, all KEY_DEF

using std::string;
using std::vector;

class RawReader{
	public:
		int InitInput();
		int UpdateInput();
		int InputLoop();

	private:
		vector<vector<struct input_event>> events;

		vector<pollfd> poll_fds;
		const int kMaxDeviceName = 80;
		vector<string> ListDir(const string &dir_path);
		bool HasKeyEvents(int device_fd);
		bool HasSpecificKey(int device_fd, unsigned int key);

		// protected virtual pour pouvoir heriter de cette class 
		// et override cette function mais cetait pr l' engine,
		// stv tu peut enlever le protected et virtual
		// (si tu change cette fonction directement sans inheritance)
	protected: 
		virtual void OnInput(int,bool);
};
