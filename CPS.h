#ifndef CPS_H_INCLUDED
#define CPS_H_INCLUDED

#include <cmath>
using std::sin;
using std::cos;
using std::abs;
#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cout;
using std::endl;
#include <initializer_list>
using std::initializer_list;
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;
#include <memory>
using std::shared_ptr;
using std::make_shared;

const double  TRISIDES = 3.0;
const double SQUARESIDES = 4.0;

const double PI = 4*atan(1);	//calculates pi to the machine's precision

class shape
{


private:

	double _height,_width;


protected:

	void setHeight(double height);
	void setWidth(double width);
	string draw(const shape &s, int x, int y);
	//this function is global, won't compile as member function
	//void findAndReplace(string & s, string && find, string && replace);
public:

	virtual double getHeight() const;
	virtual double getWidth() const;
	virtual string getPostScript() const = 0;
	virtual ~shape() = default;				
};

class spacer : public shape
{
public:
	spacer(double height, double width);

	string getPostScript() const override;
};



// for triangle, do polygon contructor but do it for 3 sides 
class circle : public shape
{
private : 
	double _radius;

public :
	circle(double rad);

	double getRad() const;

	string getPostScript() const override;
};

class polygon : public shape
{
private:
	int _numSides;
	double _sideLength;
	double _circumRad, _inRad, _innerAngle;

public:
	polygon(double numSides, double sideLength);

	double calcCircumRad();
	double calcInRad();

	double calcHeight();
	double calcWidth();
	double calcInnerAngle();

	int getNumSides() const;
	double getSideLength() const; 
	double getInnerAngle() const;

	double getCircumRad() const;
	double getInRad() const;

	string getPostScript() const override;
};


class pixel : public shape
{

private:
	string _postScript;

public:
	pixel(double r, double g, double b);

	string getPostScript() const override;

};

class mandelbrot : public shape
{

private:

	string _postScript;

public:

	mandelbrot(int width, int height);

	string getPostScript() const override;

};


class rectangle : public shape
{

public:
	rectangle(double width, double height);

	string getPostScript() const override;
	
};

class triangle : public polygon
{
public:

	triangle(double numSides, double sideLength) : polygon(TRISIDES, sideLength) {};
};

class square : public polygon
{

public:
	square(double numSides, double sideLength) : polygon(SQUARESIDES, sideLength){}

};


class rotated : public shape
{
private:

	double _origWidth, _origHeight;
	double _rotation;
	string _postScript;	

public:

	rotated(const shape &s, double rotation);

	string getPostScript() const override;

};
class scaled : public shape
{
private: 

	double _origWidth, _origHeight;
	double _xScale, _yScale;
	string _postScript;

public:

	scaled(const shape & s, double xScale, double yScale); // takes 2 scales: one for x, one for y

	string getPostScript() const override;

};

class composite : public shape //template for horizontal, vertical, and layered
{

protected:
	vector<shared_ptr<shape>> _shapes;

public:

	virtual ~composite() = default;	

	virtual void setDimensions() = 0;
	virtual string translateToStart() const = 0;
	string drawShape(int index) const;
	virtual string translateToNext(int nextIndex) const = 0;
	string getPostScript() const override;	//uses functions above

};


class vertical : public composite
{

public:

	vertical(initializer_list<shared_ptr<shape>> shapes);

	void setDimensions() override;
	string translateToStart() const override;
	string translateToNext(int nextIndex) const override;

};

class horizontal : public composite
{

public:

	horizontal(initializer_list<shared_ptr<shape>> shapes);

	void setDimensions() override;
	string translateToStart() const override;
	string translateToNext(int nextIndex) const override;

};

class layered : public composite
{

public:

	layered(initializer_list<shared_ptr<shape>> shapes);	

	void setDimensions() override;
	string translateToStart() const override;
	string translateToNext(int nextIndex) const override;
};


//file output interface

class page
{

private:

	string _postScript;

public:


	void drawTo(const shape &s, int x, int y);
	string getPostScript();
};

class output
{

private:

	string _postScript;
	vector<page> pages;

public:

	void addPage(const page &p);

	void outputFile(string fname);

};

#endif