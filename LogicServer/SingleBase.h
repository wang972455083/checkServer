#pragma once

template<typename T>
class SingleBase {
public:
	static T& getInstance() {
		static T inst;
		return inst;
	}

	virtual ~SingleBase()
	{

	}

	void Init()
	{}

protected:
	SingleBase() {}
	SingleBase(SingleBase const &) = delete;
	void operator=(SingleBase const&) = delete;
};
