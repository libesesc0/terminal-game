/*
 * I have not written this code and I do not know from where
 * is it from, a friend of mine gave it to me.
 *
 * If you have more information about it, contact me.
 */

#include "InputRawReader.h"
#include <dirent.h>					// DIR directory opendir readdir dirent 
#include <fcntl.h>					// open, O_RDONLY
#include <cstring>					//-OR- #include <string.h>
#include <unistd.h>					// read

void RawReader::OnInput(int key, bool pressed){
	printf("\t%d %s\n", key, pressed ? "pressed" : "released" );
}

int RawReader::InitInput(){
	char devicename[kMaxDeviceName];
	vector<string> filenames = ListDir("/dev/input");
	for (size_t i = 0; i < filenames.size(); i++) {
		int fd = open(filenames[i].c_str(), O_RDONLY);
		if (fd < 0) return 1;
		if(ioctl(fd, EVIOCGNAME(sizeof(devicename)-1),&devicename) < 1)
			devicename[0] = '\0';
		if (!HasKeyEvents(fd)) continue;
		if (!HasSpecificKey(fd, KEY_B)) continue;
		poll_fds.push_back(pollfd{fd, POLLIN, 0});
	}
	if (poll_fds.empty()) return 1;
	events = vector<vector<input_event>> (poll_fds.size());
	return 0;
}

int RawReader::UpdateInput(){
	poll(poll_fds.data(), poll_fds.size(), -1);
	for (size_t i = 0; i < poll_fds.size(); i++) {
		if ( !(poll_fds[i].revents & POLLIN) ) continue;
		struct input_event event;
		if (read(poll_fds[i].fd, &event, sizeof(event)) != sizeof(event))
			continue;
		if (event.type == EV_KEY )
			events[i].push_back(event);
		if ( !(event.type == EV_SYN && event.code == SYN_REPORT) )
			continue;
		for (vector<input_event>::iterator it = events[i].begin()
				;	it != events[i].end(); it++) {
			if (it->value < 0 || it->value > 1) continue;
			OnInput(it->code,it->value);
		}
		events[i].clear();
	}
	return 0;
}

vector<string> RawReader::ListDir(const string &dir_path){
	vector<string> filenames;
	DIR* directory = opendir(dir_path.c_str());
	if (!directory) return filenames;
	struct dirent *entry = NULL;
	while ((entry = readdir(directory)))
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			filenames.push_back(dir_path + "/" + entry->d_name);
	return filenames;
}

bool RawReader::HasKeyEvents(int device_fd){
	unsigned long evbit = 0; // Get bit field of available event types
	ioctl(device_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
	return evbit & (1 << EV_KEY);
}

bool RawReader::HasSpecificKey(int device_fd, unsigned int key){
	size_t nchar = KEY_MAX/8 + 1;
	unsigned char bits[nchar]; // Get bit fields of available keys
	ioctl(device_fd, EVIOCGBIT(EV_KEY, sizeof(bits)), &bits);
	return bits[key/8] & (1 << (key % 8));
}
