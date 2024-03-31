struct Macro {
private:

public:
	bool auto_reconnect = true;

	bool auto_collect = false;
	bool auto_collect_force = false;
	int auto_collect_range = 0;
	int auto_collect_interval = 0;
	uint64_t auto_collect_last = 0;

};