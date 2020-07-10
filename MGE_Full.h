#pragma once

/*
MGEngine_Full.h

Verzija 1.0.0

10072020
*/

#include<chrono>
#include <vector>
#include <string>
#include <windows.h>

namespace MGEngine {

	typedef unsigned int uint;

	template<typename T>
	struct Point2 {
		T x;
		T y;

		Point2();
		Point2(T x, T y);

		void	Init(T x, T y);
		MGEngine::Point2<T>& Offset(T x, T y);
		MGEngine::Point2<T>& Offset(Point2 P);
		MGEngine::Point2<T>		Temp_Offset(T x, T y)	const;
		MGEngine::Point2<T>		Temp_Offset(Point2 P)	const;

		bool	Compare(T x, T y)		const;
		bool	Compare(Point2<T> P)	const;

		MGEngine::Point2<T>		operator+ (Point2 Val)	const;
		MGEngine::Point2<T>		operator- (Point2 Val)	const;
		MGEngine::Point2<T>		operator* (T Val)		const;
		MGEngine::Point2<T>		operator/ (T Val)		const;

		MGEngine::Point2<T>& operator+= (Point2 Val);
		MGEngine::Point2<T>& operator-= (Point2 Val);
		MGEngine::Point2<T>& operator*= (T Val);
		MGEngine::Point2<T>& operator/= (T Val);

		bool	operator==(Point2 Val)	const;
		bool	operator!=(Point2 Val)	const;
	};

	typedef Point2<int>			Point2i;
	typedef Point2<double>		Point2d;

	double GetDistance(int x1, int y1, int x2, int y2);
	double GetDistance(double x1, double y1, double x2, double y2);
	double GetDistance(MGEngine::Point2i P1, MGEngine::Point2i P2);
	double GetDistance(MGEngine::Point2d P1, MGEngine::Point2d P2);

	struct Color {
		static const uint Black = 0;
		static const uint DarkBlue = 1;
		static const uint DarkGreen = 2;
		static const uint DarkCyan = 3;
		static const uint DarkRed = 4;
		static const uint DarkMagenta = 5;
		static const uint DarkYellow = 6;
		static const uint White = 7;
		static const uint Gray = 8;
		static const uint Blue = 9;
		static const uint Green = 10;
		static const uint Cyan = 11;
		static const uint Red = 12;
		static const uint Magenta = 13;
		static const uint Yellow = 14;
		static const uint PureWhite = 15;

		uint IgnoreMe;
	};

	struct Domain {
		MGEngine::Point2i Position;
		MGEngine::Point2i Size;

		Domain();
		Domain(int xPos, int yPos, int xSize, int ySize);
		Domain(MGEngine::Point2i Position, MGEngine::Point2i Size);
	};

	bool IsPointInDomain(const MGEngine::Point2i P, const MGEngine::Domain D);
	bool DoDomainsOverlap(const MGEngine::Domain D1, const MGEngine::Domain D2);

	bool IsValidCoordinate(MGEngine::Point2i P);

	class Timer {

		std::chrono::time_point<std::chrono::steady_clock> MyStart;
		std::chrono::time_point<std::chrono::steady_clock> MyEnd;

		void stopTimer();

	public:

		Timer();

		void StartTimer();

		double GetElapsed_seconds();
		double GetElapsed_miliseconds();
		double GetElapsed_microseconds();

	};

	struct RawScreenData {
		std::vector<CHAR_INFO> CanvasMatrix;
		std::vector<uint> ColorMatrix;
		std::vector<uint> FontColorMatrix;

		MGEngine::Point2i MatrixSize;
	};

	class Canvas {

		std::vector<CHAR_INFO> MyCanvasArray;
		std::vector<uint> MyColorArray;
		std::vector<uint> MyFontColorArray;

		MGEngine::Point2i MyRealSize;

		HANDLE MyOutput;
		HANDLE MyInput;

		CONSOLE_SCREEN_BUFFER_INFOEX MyBufferInfo;

		int	indexOf(int x, int y);
		int	indexOf(MGEngine::Point2i P);

		void	updateCanvasColors();

		void	checkCursorPos();

		std::string	loadUserInput();

		MGEngine::Point2i getRealMousePos();

		MGEngine::Point2i MyCurrentCursorPos;
		uint MyFontColor = 7;

		bool NeedsUpdating = true;

	public:

		Canvas(int xSize, int ySize);
		Canvas(MGEngine::Point2i Size);
		~Canvas();

		MGEngine::Canvas& SetFontColor(uint Color);
		MGEngine::Canvas& SetCursorPosition(int xPos, int yPos, bool UsePixelPos = true);
		MGEngine::Canvas& SetCursorPosition(MGEngine::Point2i Pos, bool UsePixelPos = true);
		MGEngine::Canvas& SetPixel(uint Color, int xPos, int yPos);
		MGEngine::Canvas& SetPixel(uint Color, MGEngine::Point2i Pos);

		uint GetFontColor();
		uint GetPixel(int xPos, int yPos);
		uint GetPixel(MGEngine::Point2i Pos);

		MGEngine::Point2i	GetSize();
		MGEngine::Domain	GetDomain();
		MGEngine::Point2i	GetCursorPositon(bool UsePixelPos = true);

		MGEngine::Point2i	GetMousePosition();
		bool				GetMouseLeftClickState();
		bool				GetMouseRightClickState();

		MGEngine::Canvas& operator<<(const char C);

		MGEngine::Canvas& operator<<(const std::string Val);
		MGEngine::Canvas& operator<<(const char* Val);
		MGEngine::Canvas& operator<<(const bool Val);
		MGEngine::Canvas& operator<<(const int Val);
		MGEngine::Canvas& operator<<(const float Val);
		MGEngine::Canvas& operator<<(const double Val);

		MGEngine::Canvas& operator<<(const MGEngine::Color Val);
		MGEngine::Canvas& operator<<(const MGEngine::Point2i Val);
		void	operator<<(const MGEngine::RawScreenData RSD);

		bool	operator>>(std::string& Val);
		bool	operator>>(bool& Val);
		bool	operator>>(int& Val);
		bool	operator>>(char& Val);
		bool	operator>>(float& Val);
		bool	operator>>(double& Val);
		bool	operator>>(RawScreenData& Val);

		void ClearCanvas(uint Color = 0);
		void Draw();

	};

	MGEngine::Color SetFontColor(uint ColorVal);

	void PlaceTextBlock(MGEngine::Canvas& MyCan, std::string Text, MGEngine::uint Color, int xPos, int yPos);
	void PlaceTextBlock(MGEngine::Canvas& MyCan, std::string Text, MGEngine::uint Color, MGEngine::Point2i Pos);

	const char endline = '\n';

	void AppendLine(MGEngine::Canvas& MyCan, uint LineCol, int x1, int y1, int x2, int y2);
	void AppendLine(MGEngine::Canvas& MyCan, uint LineCol, MGEngine::Point2i P1, MGEngine::Point2i P2);

	class GenericShape {

	protected:
		MGEngine::Canvas* MyCanvas;
		MGEngine::Domain MyDomain;

		uint MyColorOuter = 0;
		uint MyColorInner = 0;

		bool MyFillMode = true;


	public:

		GenericShape(MGEngine::Canvas* Can);
		GenericShape(MGEngine::Canvas* Can, MGEngine::Domain D);
		GenericShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize);
		GenericShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size);

		MGEngine::GenericShape& SetSize(int xSize, int ySize);
		MGEngine::GenericShape& SetSize(MGEngine::Point2i Size);
		MGEngine::GenericShape& SetPosition(int xPos, int yPos);
		MGEngine::GenericShape& SetPosition(MGEngine::Point2i Pos);

		MGEngine::GenericShape& SetFillColor(uint Color);
		MGEngine::GenericShape& SetOutlineColor(uint Color);
		MGEngine::GenericShape& SetInnerColor(uint Color);

		MGEngine::GenericShape& SetFillMode(bool Fill);

		MGEngine::Point2i		GetPosition();
		MGEngine::Point2i		GetSize();
		MGEngine::Domain		GetDomain();
		MGEngine::uint			GetOuterColor();
		MGEngine::uint			GetInnerColor();

		virtual void			Append() = 0;

	};

	class BoxShape : public GenericShape {

	public:

		BoxShape(MGEngine::Canvas* Can);
		BoxShape(MGEngine::Canvas* Can, MGEngine::Domain D);
		BoxShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize);
		BoxShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size);

		void Append();

	};

	class CircleShape : public GenericShape {

	public:

		CircleShape(MGEngine::Canvas* Can);
		CircleShape(MGEngine::Canvas* Can, MGEngine::Domain D);
		CircleShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize);
		CircleShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size);

		void Append();

	};

	class GUI_Button {

		MGEngine::Canvas* MyCanvas;

		MGEngine::Point2i	MyPos;
		MGEngine::Point2i	MySize;

		MGEngine::uint	MyColor = 0;
		MGEngine::uint	MySelectedColor = 0;
		MGEngine::uint	MyTextColor = 7;

		MGEngine::uint	MyTextOffset = 0;
		std::string		MyButtonText;

		bool	NeedCalculatingOffset = true;
		void	calcTextOff();

	public:
		GUI_Button(MGEngine::Canvas* Can);

		MGEngine::GUI_Button& SetSize(int xSize, int ySize);
		MGEngine::GUI_Button& SetSize(MGEngine::Point2i Size);
		MGEngine::GUI_Button& SetPosition(int xPos, int yPos);
		MGEngine::GUI_Button& SetPosition(MGEngine::Point2i Pos);

		MGEngine::GUI_Button& SetDomain(MGEngine::Domain Dom);
		MGEngine::GUI_Button& SetDomain(int xPos, int yPos, int xSize, int ySize);

		MGEngine::GUI_Button& SetColor(MGEngine::uint Color);
		MGEngine::GUI_Button& SetSelectedColor(MGEngine::uint Color);
		MGEngine::GUI_Button& SetTextColor(MGEngine::uint Color);

		MGEngine::GUI_Button& operator<<(std::string str);
		MGEngine::GUI_Button& operator<<(const char* str);
		MGEngine::GUI_Button& operator<<(bool str);
		MGEngine::GUI_Button& operator<<(int str);
		MGEngine::GUI_Button& operator<<(char str);
		MGEngine::GUI_Button& operator<<(float str);
		MGEngine::GUI_Button& operator<<(double str);

		MGEngine::Point2i	GetSize();
		MGEngine::Point2i	GetPosition();
		MGEngine::Domain	GetDomain();
		MGEngine::uint		GetColor();
		MGEngine::uint		GetSelectedColor();
		MGEngine::uint		GetTextColor();

		bool	IsHovered();
		bool	IsPressed();
		void	ClearText();
		void	Append();
	};

	class GUI_ButtonList {

		std::vector<MGEngine::GUI_Button> MyButtonArray;

		MGEngine::Canvas* MyCanvas;
		MGEngine::Point2i MyPos;
		MGEngine::Point2i MySize;

		MGEngine::Point2i MyNextButtonPos;

		MGEngine::uint MyColor = 0;
		MGEngine::uint MySelectedColor = 0;
		MGEngine::uint MyTextColor = 7;

		int MySpaceBetweenButtons = 1;

		bool MyButtonsWereChanged = true;

		void setupAllTheButtons();

	public:

		GUI_ButtonList(MGEngine::Canvas* Can);

		MGEngine::GUI_ButtonList& SetButtonSize(int xSize, int ySize);
		MGEngine::GUI_ButtonList& SetButtonSize(MGEngine::Point2i Size);

		MGEngine::GUI_ButtonList& SetListOrigin(int xPos, int yPos);
		MGEngine::GUI_ButtonList& SetListOrigin(MGEngine::Point2i Pos);

		MGEngine::GUI_ButtonList& SetColor(MGEngine::uint Color);
		MGEngine::GUI_ButtonList& SetTextColor(MGEngine::uint Color);
		MGEngine::GUI_ButtonList& SetSelectedColor(MGEngine::uint Color);

		MGEngine::GUI_ButtonList& SetSpaceBetweenButtons(int Val);
		MGEngine::GUI_ButtonList& SetButtonPlacementOrder(int x, int y);

		void AddEmptyButton();
		void RemoveButton(MGEngine::uint index);

		void AddTextToButton(MGEngine::uint ButtonIndex, std::string str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const char* str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const bool str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const int str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const char str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const float str);
		void AddTextToButton(MGEngine::uint ButtonIndex, const double str);

		void operator<<(std::string str);
		void operator<<(const char* str);
		void operator<<(const bool str);
		void operator<<(const int str);
		void operator<<(const char str);
		void operator<<(const float str);
		void operator<<(const double str);

		bool operator>>(int& Selected);
		bool GetSelectedButton(int* ButtonIndex);
		bool GetHoveredButton(int* ButtonIndex);

		MGEngine::Point2i GetListOrigin();
		MGEngine::Domain GetListDomain();
		MGEngine::Point2i GetButtonSize();
		MGEngine::Domain GetButtonDomain(MGEngine::uint ButtonIndex);

		MGEngine::uint GetColor();
		MGEngine::uint GetTextColor();
		MGEngine::uint GetSelectedColor();
		MGEngine::uint GetButtonCount();

		int GetSpaceBetweenButtons();

		void ClearButtonText(MGEngine::uint ButtonIndex);
		void DestroyAllButtons();

		void Append();
	};

	class Sprite {

		MGEngine::Canvas* MyCan;

		std::vector<MGEngine::uint> MyColorArray;

		MGEngine::Point2i MySize;
		MGEngine::Point2i MyPos;

		MGEngine::uint MyIgnoredColor = 0;

		int indexOf(int x, int y);
		int indexOf(MGEngine::Point2i P);

		void resizeVector();

	public:

		Sprite(MGEngine::Canvas* Can);
		Sprite(MGEngine::Canvas* Can, int xSize, int ySize);
		Sprite(MGEngine::Canvas* Can, MGEngine::Point2i Size);

		MGEngine::Sprite& SetSize(int xSize, int ySize);
		MGEngine::Sprite& SetSize(MGEngine::Point2i Size);
		MGEngine::Sprite& SetPosition(int xPos, int yPos);
		MGEngine::Sprite& SetPosition(MGEngine::Point2i Pos);

		MGEngine::Sprite& SetPixel(MGEngine::uint Color, int x, int y);
		MGEngine::Sprite& SetPixel(MGEngine::uint Color, MGEngine::Point2i P);

		MGEngine::Sprite& SetIgnoredColor(MGEngine::uint Color);

		MGEngine::Point2i GetSize();
		MGEngine::Point2i GetPosition();

		MGEngine::Domain GetDomain();

		MGEngine::uint GetPixel(int x, int y);
		MGEngine::uint GetPixel(MGEngine::Point2i P);

		MGEngine::uint GetIgnoredColor();

		void LoadFromFile(std::string FileName);
		void SaveToFile(std::string FileName);

		void ClearSprite();

		void Append(bool UseIgnoreColor = true);

	};

	class Animation {

		MGEngine::Canvas* MyCan;

		std::vector<MGEngine::Sprite> MySpriteArray;

		MGEngine::Point2i MyPos;
		MGEngine::Point2i MySize;

		MGEngine::uint CurrentFrame = 0;

	public:

		Animation(MGEngine::Canvas* Can, std::string FileName);
		Animation(MGEngine::Canvas* Can, int xSize, int ySize);
		Animation(MGEngine::Canvas* Can, MGEngine::Point2i Size);

		MGEngine::Animation& SetPosition(int xPos, int yPos);
		MGEngine::Animation& SetPosition(MGEngine::Point2i Pos);

		void AddNewFrame();
		void RemoveFrame(MGEngine::uint FrameIndex);
		void RemoveLast();

		void SetFrame(MGEngine::uint FrameIndex, MGEngine::Sprite& spr);
		void SetPixel(MGEngine::uint FrameIndex, MGEngine::uint Color, int x, int y);
		void SetPixel(MGEngine::uint FrameIndex, MGEngine::uint Color, MGEngine::Point2i P);

		MGEngine::uint GetPixel(MGEngine::uint FrameIndex, int x, int y);
		MGEngine::uint GetPixel(MGEngine::uint FrameIndex, MGEngine::Point2i P);

		int GetFrameCount();

		MGEngine::Point2i GetSize();
		MGEngine::Point2i GetPosition();
		MGEngine::Domain GetDomain();

		void SetActiveFrame(MGEngine::uint FrameIndex);
		void NextFrame();
		void LastFrame();

		void LoadFromFile(std::string FileName);
		void SaveToFile(std::string FileName);

		void Append();

	};

	template<typename T>
	inline Point2<T>::Point2() {
		this->Init(0, 0);
	}

	template<typename T>
	inline Point2<T>::Point2(T x, T y) {
		this->Init(x, y);
	}

	template<typename T>
	inline void Point2<T>::Init(T x, T y) {
		this->x = x;
		this->y = y;
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::Offset(T x, T y) {
		return this->Offset(MGEngine::Point2<T>(x, y));
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::Offset(Point2 P) {
		this->operator+=(P);
		return *this;
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::Temp_Offset(T x, T y) const {
		return this->Temp_Offset(MGEngine::Point2<T>(x, y));
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::Temp_Offset(Point2 P) const {
		return this->operator+(P);
	}

	template<typename T>
	inline bool Point2<T>::Compare(T x, T y) const {
		return this->Compare(MGEngine::Point2<T>(x, y));
	}

	template<typename T>
	inline bool Point2<T>::Compare(Point2<T> P) const {
		return this->operator==(P);
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::operator+(Point2 Val) const {
		MGEngine::Point2<T> TempPoint;
		TempPoint.x = this->x + Val.x;
		TempPoint.y = this->y + Val.y;

		return TempPoint;
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::operator-(Point2 Val) const {
		MGEngine::Point2<T> TempPoint;
		TempPoint.x = this->x - Val.x;
		TempPoint.y = this->y - Val.y;

		return TempPoint;
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::operator*(T Val) const {
		MGEngine::Point2<T> TempPoint;
		TempPoint.x = this->x * Val;
		TempPoint.y = this->y * Val;

		return TempPoint;
	}

	template<typename T>
	inline MGEngine::Point2<T> Point2<T>::operator/(T Val) const {
		MGEngine::Point2<T> TempPoint;
		TempPoint.x = this->x / Val;
		TempPoint.y = this->y / Val;

		return TempPoint;
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::operator+=(Point2 Val) {
		*this = this->operator+(Val);
		return *this;
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::operator-=(Point2 Val) {
		*this = this->operator-(Val);
		return *this;
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::operator*=(T Val) {
		*this = this->operator*(Val);
		return *this;
	}

	template<typename T>
	inline MGEngine::Point2<T>& Point2<T>::operator/=(T Val) {
		*this = this->operator/(Val);
		return *this;
	}

	template<typename T>
	inline bool Point2<T>::operator==(Point2 Val) const {
		if (this->x != Val.x) return false;
		if (this->y != Val.y) return false;
		return true;
	}

	template<typename T>
	inline bool Point2<T>::operator!=(Point2 Val) const {
		return !this->operator==(Val);
	}

}