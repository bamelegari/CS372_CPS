#include "CPS.h"


double shape::getHeight() const 
{
	return _height;
}
double shape::getWidth() const
{
	return _width;
}

void shape::setHeight(double height)
{
	_height = height;
}
void shape::setWidth(double width)
{
	_width = width;
}

rectangle::rectangle(double width,double height)
{
	setWidth(width);
	setHeight(height);
}

//this probably needs a better place than global
//but it is a nice way of setting postscript strings
//size of replace must be < size of find
void findAndReplace(string & s, string && find, string && replace)
{

	if(replace.length() < find.length())
	{
		string temp;
		temp.resize(find.length(), ' ');

		for(int i = 0; i < replace.length(); i++)
		{
			temp[i] = replace[i];
		}

		replace = temp;
	}

	while(true)
	{
		auto found = s.find(find);

		if(found == string::npos) break;	//find not found
		else
		{
			s.replace(found, find.length(), replace);
		}
	}

}

//note: I move the cursor to 4, 5.5 at the beginning of these just to get
//it somewhere visible because 0,0 is the bottom left corner. We'll go back
//and figure out where the cursor should go in relation to the center later.

string circle::getPostScript() const
{
	string ret = R"(
		newpath
		4 inch 5.5 inch RAD 0 360 arc
	)";
	findAndReplace(ret, "RAD", to_string( (int)getRad() ));

	return ret;
}

string rectangle::getPostScript() const 
{						//setting convention now that inch will needs to be defined in postscript file header
	string ret = R"(
		newpath
		0 0 moveto
		WIDTH 0 rlineto
		0 HEIGHT rlineto
		0 WIDTH sub 0 rlineto
		closepath
	)";

	findAndReplace(ret, "WIDTH", to_string( (int)getWidth() ));
	findAndReplace(ret, "HEIGHT", to_string( (int)getHeight() ));

	return ret;
}

string polygon::getPostScript() const
{
	//polygons will start drawing at the bottom, since we know all
	//polygons will have a horizontal bottom edge.
	string ret = R"(
		newpath

		144 144 moveto
		1 1 SIDESMINUSONE{
			SIDELENGTH 0 rlineto
			ROTATIONANGLE rotate
		} for

		closepath
		stroke
		showpage
	)";

	double sidesminusone = getNumSides() - 1;
	double rotationangle = 180 - getInnerAngle();

	findAndReplace(ret, "SIDELENGTH", to_string(getSideLength()));
	findAndReplace(ret, "SIDESMINUSONE", to_string(sidesminusone));
	findAndReplace(ret, "ROTATIONANGLE", to_string(rotationangle));

	return ret;
}

rotated::rotated(const shape &s, double rotation) : _rotation(rotation), _postScript(s.getPostScript())
{
	//need to limit to 90 degree intervals...or this could handle all rotations technically

	//const double pi = 3.1415926;
	const double radians = _rotation * 2.0 * PI / 360.0;

	_origWidth = s.getWidth();
	_origHeight = s.getHeight();

	setWidth(abs(_origWidth * cos(radians) + _origHeight * sin(radians)));
	setHeight(abs(_origHeight * cos(radians) + _origWidth * sin(radians)));


}

//this is much simpler because you just prefix the rotate command
string rotated::getPostScript() const
{
	string ret;

	ret = to_string( (int)_rotation);

	ret += " rotate \n";
	ret += _postScript;

	return ret;
}

int main()
{
	triangle tri(5, 6.0);
	square sq(5, 6.0);
	std::cout << "triangle number of sides " <<tri.getNumSides() << "\n" << "triangle side length "  <<   tri.getSideLength() << std::endl;
	std::cout << "square number of sides " << sq.getNumSides() << std::endl;

	rectangle rect(40,20);
	cout<< "rectangle dimensions " <<rect.getWidth() << " by " << rect.getHeight() << endl;
	cout<< rect.getPostScript() << endl;

	rotated rot(rect, 270);

	cout<< "rotated dimensions " << rot.getWidth() << " by " << rot.getHeight() << endl;
	cout<< rot.getPostScript() << endl;

//	having problem with init lists, this causes seg fault
//	vertical vert{rect, rot, rect};
//	cout<<vert.getPostScript() << endl;

	page test;
	test.drawTo(rect, 2,2);
	test.drawTo(rot, 4,4);
	cout<<"draw shapes to page \n";
	cout<<test.getPostScript() <<endl;


	output of;
	of.addPage(test);
	cout<<"testing file output \n";
	of.outputFile("test.ps");



	return 0;
}

double circle::getRad() const
{
	return _radius;
}

double polygon::calcCircumRad()
{
	//side length of an n-sided regular polygon with circumradius cr:
	// L = 2*cr*sin(180/n)
	//because we know L and n, solve that equation for cr: 
	// cr = L/(2*sin(180/n))

	return getSideLength() / (2*sin(180/getNumSides()));
}

double polygon::calcInRad()
{
	//inRad ir given sides, and circumRad cr:
	// ir = cr*sin(pi/n)
	//in the c'tor, calcCircumRad is called first.

	return getCircumRad()*sin(PI/getNumSides());
}

double polygon::calcHeight()
{
	//note: the following statements assume flat side on the bottom
	//if number of sides is odd, h = ir + cr
	//if number of sides is even, h = ir*2

	int sides = getNumSides();

	if (sides%2 == 0)	//even # of sides
	{
		return getInRad()*2;		
	}
	else	//odd # of sides
	{
		return getInRad() + getCircumRad();
	}

}

double polygon::calcWidth()
{
	//note: the following statements assume flat side on the bottom

	//for n=3, width is side length
	//starting at n=4, every 4 polygons (8, 12, etc.) have width ir*2
	//starting at n=6, every 4 polygons (10, 14, etc.) have width cr*2
	
	//for the odds, the width is the chord of the circular segment 
	//between (n+1)/2 vertices. The angle for this segment is that
	//number of vertices times 360/n (the angle subtending a side)
	// chord length = 2*cr*sin(360/n*numVertices) = width

	int n = getNumSides();

	if (n == 3)
	{
		return getSideLength();
	}

	else if (n % 4 == 0)
	{
		return getInRad()*2;
	}

	else if (n % 2 == 0)	//this also implies n % 4 != 0, because of the order
	{
		return getCircumRad()*2;
	}

	else	//n is odd and != 3
	{
		int numVertices = (n + 1)/2;
		double theta = 360/n * numVertices;
		return 2*getCircumRad()*sin(theta);
	}
	
}

double polygon::calcInnerAngle()
{
	//one of the inner angles of a polygon = (180*(n-2))/n
	int n = getNumSides();
	return 180*(n - 2)/n;
}

double polygon::getInRad() const
{
	return _inRad;
}

double polygon::getCircumRad() const
{
	return _circumRad;
}

double polygon::getInnerAngle() const
{
	return _innerAngle;
}

int polygon::getNumSides() const
{
	return _numSides;
}

double polygon::getSideLength() const
{
	return _sideLength;
}

//another global test function that needs moved eventually
string draw(const shape &s, int x, int y)
{
	string ret;

	ret += "gsave \n";
	ret += to_string( (int) x) + " inch " + to_string( (int) y) + " inch translate\n";
	ret += s.getPostScript();
	ret += "\n stroke \n grestore \n";

	return ret;
}

string vertStackOdd(const vector<shape> &shapes)
{
	string ret;



	int mid = shapes.size()/2;

	int rtOffset = 0, ltOffset = 0;

	//draw middle shape
	ret = draw(shapes[mid], 0, 0);

	//draw surrounding shapes
	for(int i = 0; i < mid; i++)
	{

		rtOffset += shapes[mid+i].getWidth()/2;
		ltOffset += shapes[mid-i].getWidth()/2;

		ret += draw(shapes[mid+i], rtOffset, 0);
		ret += draw(shapes[mid-i], ltOffset, 0);		

		rtOffset += shapes[mid+i].getWidth()/2;
		ltOffset += shapes[mid-i].getWidth()/2;

	}

	return ret;
}

vertical::vertical(initializer_list<shape> shapes)
{
	double sumWidth = 0, maxHeight = 0;

	for(auto s : shapes)
	{
		cout<<"DEBUG: \n";
		cout<<s.getPostScript()<<endl;
		sumWidth += s.getWidth();
		if(s.getHeight() > maxHeight)
			maxHeight = s.getHeight();
	}

	setWidth(sumWidth);
	setHeight(maxHeight);




	if(shapes.size()%2==0)	//even number of shapes
	{
		//split an even in half to get 2 odds

		auto midPtr = shapes.begin();

		midPtr = midPtr + shapes.size()/2;

		vector<shape> lefts(shapes.begin(), midPtr ),
  						 rights(midPtr, shapes.end());


		_postScript = vertStackOdd(lefts) + vertStackOdd(rights);

	}
	else
	{

		vector<shape> list = shapes;
		//cout<<list[1].getPostScript()<<endl;
		_postScript = vertStackOdd(shapes);
	}


}

string vertical::getPostScript()
{
	return _postScript;
}

void page::drawTo(const shape &s, int x, int y)
{
	_postScript += "gsave \n";
	_postScript += to_string( (int) x) + " inch " + to_string( (int) y) + " inch translate\n";
	_postScript += s.getPostScript();
	_postScript += "\n stroke \n grestore \n";
}

string page::getPostScript()
{ 
	return _postScript;
}

void output::addPage(const page &p)
{
	pages.push_back(p);
}

void output::outputFile(string fname)
{

	ofstream ofs(fname);

	ofs<<"%1 \n /inch {72 mul} def \n";

	for(auto i : pages)
	{
		ofs<<i.getPostScript();
		ofs<<" \n showpage \n";
	}

	ofs.close();
}