#ifndef SUPERMARIOCLONE_H_
#define SUPERMARIOCLONE_H_

#define LMK_INCLUDE_ALL
#include "LMK/LMK.h"

class SuperMarioClone : public lmk::lmkEngine {
public:	// Constructors & Destructors
	SuperMarioClone(int _x, int _y, int _w, int _h, bool _fullscr = false)	
		: lmkEngine("Super Mario Clone", _x, _y, _w, _h, _fullscr) {}

public: // Functions
	inline virtual void UserHandleEvents(const SDL_Event& _event) override {

	}

	inline virtual void UserUpdate() override {

	}

	inline virtual void UserRender() override {

	}

private:

private: // Properties
};

#endif // !SUPERMARIOCLONE_H_