#ifndef BOT_INTERFACE_H
#define BOT_INTERFACE_H
#include "kf/kf_vector2.h"
#include "vector"

class VisibleThing
{
public:
	enum ThingTypes
	{
		e_robot = 0,
		e_bullet
	};
	ThingTypes type;
	std::string name;
	kf::Vector2 position;
};

class Line
{
public:
	Line():start(0,0),end(0,0),r(1), g(1), b(1)
	{
	}
	Line(const kf::Vector2 &s, const kf::Vector2 &e, float red=1, float green=1, float blue=1):start(s),end(e),r(red), g(green), b(blue)
	{
	}
	kf::Vector2 start;
	kf::Vector2 end;
	float r,g,b;
};


class BotInput
{
public:
	kf::Vector2 position;
	kf::Vector2 velocity;
	int health;
	int healthMax;
	float bulletSpeed;
	std::vector<VisibleThing> scanResult;
};

class BotOutput
{
public:
	enum Action
	{
		scan=0,
		shoot
	};
	kf::Vector2 moveDirection;
	float motor;
	kf::Vector2 lookDirection;
	int action;
	std::vector<Line> lines;
};

class TextMsg
{
public:
	TextMsg(const std::string &s, const kf::Vector2 &pos, float rr = 1.0f, float gg = 1.0f, float bb = 1.0f, float charsize = 30.0f) : text(s), size(charsize), position(pos), r(rr), g(gg), b(bb) {}
	std::string text;
	float size;
	kf::Vector2 position;
	float r, g, b;
};

class BotOutput27: public BotOutput
{
public:
	BotOutput27() : spriteFrame(0) {}
	std::vector < TextMsg > text;
	unsigned int spriteFrame;
};

#pragma pack(push, 1)
class Tile
{
public:
	float damping;
	bool wall;
	unsigned char type;
};
#pragma pack(pop)

class MapData
{
public:
	MapData():width(0),height(0),data(0) {}
	~MapData() {
		//delete [] data;
	}
	MapData & operator=(const MapData &m)
	{
		width = m.width;
		height = m.height;
		delete [] data;
		data = new Tile[width*height];
		memcpy(data,m.data,width*height*sizeof(Tile));
		return *this;
	}
	int width;
	int height;
	Tile *data;
};

class BotInitialData
{
public:
	std::string mapName;
	MapData mapData;
	float scanFOV;
	std::vector<std::pair<std::string, std::string> > properties;
};

class BotAttributes
{
public:
	float motor;
	float health;
	float weaponSpeed;
	float weaponStrength;
};

class BotInterface
{
public:
	BotInterface() {};
	virtual ~BotInterface() {};
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib) = 0;
	virtual void update(const BotInput &input, BotOutput &output) = 0;
	virtual void result(bool won) = 0;
};

class BotInterface27
{
public:
	BotInterface27() {};
	virtual ~BotInterface27() {};
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib) = 0;
	virtual void update(const BotInput &input, BotOutput27 &output) = 0;
	virtual void result(bool won) = 0;
	virtual void bulletResult(bool hit) {}
};

typedef BotInterface *(*CreateBotFunc)();
typedef BotInterface27 *(*CreateBotFunc27)();

#endif
