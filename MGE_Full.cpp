#include "MGE_Full.h"

#include <cmath>
#include <sstream>
#include <fstream>
#include <conio.h>
#include <iostream>
#include <algorithm>

#define IgnoreChar 200

bool DoesLineIverlapWithRegion(int LineStart, int LineEnd, int RegionStart, int RegionEnd) {
    if (LineStart <= RegionStart && LineEnd >= RegionStart) return true;
    else if (LineStart <= RegionEnd && LineEnd >= RegionEnd) return true;
    else return false;
}

double MGEngine::GetDistance(int x1, int y1, int x2, int y2) {
    return MGEngine::GetDistance(MGEngine::Point2i(x1, y1), MGEngine::Point2i(x2, y2));
}

double MGEngine::GetDistance(double x1, double y1, double x2, double y2) {
    return MGEngine::GetDistance(MGEngine::Point2d(x1, y1), MGEngine::Point2d(x2, y2));
}

double MGEngine::GetDistance(MGEngine::Point2i P1, MGEngine::Point2i P2) {
    return std::sqrt(std::pow(P1.x - P2.x, 2) + std::pow(P1.y - P2.y, 2));
}

double MGEngine::GetDistance(MGEngine::Point2d P1, MGEngine::Point2d P2) {
    return std::sqrt(std::pow(P1.x - P2.x, 2) + std::pow(P1.y - P2.y, 2));
}

bool MGEngine::IsPointInDomain(const MGEngine::Point2i P, const MGEngine::Domain D) {
    if (P.x >= D.Position.x && P.x <= (D.Position + D.Size).x) {
        if (P.y >= D.Position.y && P.y <= (D.Position + D.Size).y) {
            return true;
        }
    }
    return false;
}

bool MGEngine::DoDomainsOverlap(const MGEngine::Domain D1, const MGEngine::Domain D2) {

    if (DoesLineIverlapWithRegion(D1.Position.x, (D1.Position + D1.Size).x, D2.Position.x, (D2.Position + D2.Size).x)) {
        if (DoesLineIverlapWithRegion(D1.Position.y, (D1.Position + D1.Size).y, D2.Position.y, (D2.Position + D2.Size).y)) {
            return true;
        }
    }
    return false;

}

bool MGEngine::IsValidCoordinate(MGEngine::Point2i P) {
    return !P.Compare(-1, -1);
}

MGEngine::Domain::Domain() {
    this->Position.Init(0, 0);
    this->Size.Init(0, 0);
}

MGEngine::Domain::Domain(int xPos, int yPos, int xSize, int ySize) {
    this->Position.Init(xPos, yPos);
    this->Size.Init(xSize, ySize);
}

MGEngine::Domain::Domain(MGEngine::Point2i Position, MGEngine::Point2i Size) {
    this->Position = Position;
    this->Size = Size;
}

void MGEngine::Timer::stopTimer() {
    this->MyEnd = std::chrono::high_resolution_clock::now();
}

MGEngine::Timer::Timer() {
    MyStart = std::chrono::high_resolution_clock::now();
}

void MGEngine::Timer::StartTimer() {
    this->MyStart = std::chrono::high_resolution_clock::now();
}

double MGEngine::Timer::GetElapsed_seconds() {
    this->stopTimer();
    return std::chrono::duration_cast<std::chrono::seconds>(this->MyEnd - this->MyStart).count();
}

double MGEngine::Timer::GetElapsed_miliseconds() {
    this->stopTimer();
    return std::chrono::duration_cast<std::chrono::milliseconds>(this->MyEnd - this->MyStart).count();
}

double MGEngine::Timer::GetElapsed_microseconds() {
    this->stopTimer();
    return std::chrono::duration_cast<std::chrono::microseconds>(this->MyEnd - this->MyStart).count();
}

int MGEngine::Canvas::indexOf(int x, int y) {
    return y * this->MyRealSize.x + x;
}

int MGEngine::Canvas::indexOf(MGEngine::Point2i P) {
    return this->indexOf(P.x, P.y);
}

void MGEngine::Canvas::updateCanvasColors() {
    for (int y = 0; y < this->MyRealSize.y; ++y) {
        for (int x = 0; x < this->MyRealSize.x; ++x) {
            int index = this->indexOf(x, y);

            if (this->MyCanvasArray[index].Char.UnicodeChar == IgnoreChar) {
                this->MyCanvasArray[index].Attributes = this->MyColorArray[index] * 16 + this->MyColorArray[index];
            }
            else {
                this->MyCanvasArray[index].Attributes = this->MyColorArray[index] * 16 + this->MyFontColorArray[index];
            }
        }
    }
}

void MGEngine::Canvas::checkCursorPos() {
    if (this->MyCurrentCursorPos.x >= this->MyRealSize.x) {
        this->MyCurrentCursorPos.x = 0;
        this->MyCurrentCursorPos.y++;
    }

    if (this->MyCurrentCursorPos.y >= this->MyRealSize.y) {
        this->MyCurrentCursorPos.Init(0, 0);
    }
}

std::string MGEngine::Canvas::loadUserInput() {
    this->Draw();

    std::string UserInput;
    char Input;

    while (1) {
        Input = _getwch();

        if (Input == 13) break;
        else if (Input == 8) {  //Backspace
            UserInput.pop_back();

            MGEngine::Point2i Pos = this->MyCurrentCursorPos;
            Pos.Offset(-1, 0);

            this->MyCurrentCursorPos = Pos;
            this->operator<<(' ');
            this->MyCurrentCursorPos = Pos;

            this->Draw();
        }
        else {
            this->operator<<(Input);
            this->Draw();
            UserInput.push_back(Input);
        }

    }

    this->operator<<('\n');
    return UserInput;
}

MGEngine::Point2i MGEngine::Canvas::getRealMousePos() {
    POINT P;
    GetCursorPos(&P);

    RECT R;
    GetWindowRect(GetConsoleWindow(), &R);

    MGEngine::Point2i MousePos;
    MousePos.x = P.x - R.left - 8;
    MousePos.y = P.y - R.top - 32;

    return MousePos;
}

MGEngine::Canvas::Canvas(int xSize, int ySize) : Canvas(MGEngine::Point2i(xSize, ySize)) {}

MGEngine::Canvas::Canvas(MGEngine::Point2i Size) {
    this->MyRealSize = Size;
    this->MyRealSize.x *= 2;

    this->MyCanvasArray.resize(this->MyRealSize.x * this->MyRealSize.y);
    this->MyColorArray.resize(this->MyRealSize.x * this->MyRealSize.y);
    this->MyFontColorArray.resize(this->MyRealSize.x * this->MyRealSize.y);

    this->ClearCanvas();

    this->MyOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    this->MyInput = GetStdHandle(STD_INPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFOEX CSBI;
    CSBI.cbSize = sizeof(CSBI);

    GetConsoleScreenBufferInfoEx(this->MyOutput, &CSBI);
    this->MyBufferInfo = CSBI;

    CSBI.dwSize.X = this->MyRealSize.x;
    CSBI.dwSize.Y = this->MyRealSize.y;

    CSBI.srWindow.Right = this->MyRealSize.x * 8;
    CSBI.srWindow.Bottom = this->MyRealSize.y * 16;

    SetConsoleScreenBufferInfoEx(this->MyOutput, &CSBI);

    SetConsoleMode(this->MyInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT);
}

MGEngine::Canvas::~Canvas() {
    SetConsoleScreenBufferInfoEx(this->MyOutput, &this->MyBufferInfo);
}

MGEngine::Canvas& MGEngine::Canvas::SetPixel(uint Color, int xPos, int yPos) {
    if (xPos >= 0 && xPos < this->GetSize().x && yPos >= 0 && yPos < this->GetSize().y) {
        this->MyColorArray[this->indexOf(xPos * 2, yPos)] = Color;
        this->MyColorArray[this->indexOf(xPos * 2 + 1, yPos)] = Color;
        this->NeedsUpdating = true;
    }

    return *this;
}

MGEngine::Canvas& MGEngine::Canvas::SetPixel(uint Color, MGEngine::Point2i Pos) {
    return this->SetPixel(Color, Pos.x, Pos.y);
}

MGEngine::Canvas& MGEngine::Canvas::SetFontColor(uint Color) {
    if (Color >= 16) Color = 7;
    this->MyFontColor = Color;

    return *this;
}

MGEngine::Canvas& MGEngine::Canvas::SetCursorPosition(int xPos, int yPos, bool UsePixelPos) {
    return this->SetCursorPosition(MGEngine::Point2i(xPos, yPos), UsePixelPos);
}

MGEngine::Canvas& MGEngine::Canvas::SetCursorPosition(MGEngine::Point2i Pos, bool UsePixelPos) {
    if (Pos.x >= 0 && Pos.x <= this->MyRealSize.x && Pos.y >= 0 && Pos.y <= this->MyRealSize.y) {
        MGEngine::Point2i temp = Pos;

        if (UsePixelPos) temp.x *= 2;

        this->MyCurrentCursorPos = temp;
    }
    return *this;
}

MGEngine::uint MGEngine::Canvas::GetPixel(int xPos, int yPos) {
    if (MGEngine::IsPointInDomain(MGEngine::Point2i(xPos, yPos), this->GetDomain())) {
        int index = this->indexOf(xPos * 2, yPos);

        return this->MyColorArray[index];
    }
    return 0;
}

MGEngine::uint MGEngine::Canvas::GetPixel(MGEngine::Point2i Pos) {
    return this->GetPixel(Pos.x, Pos.y);
}

MGEngine::uint MGEngine::Canvas::GetFontColor() {
    return this->MyFontColor;
}

MGEngine::Point2i MGEngine::Canvas::GetSize() {
    MGEngine::Point2i Size = this->MyRealSize;

    Size.x /= 2;

    return Size;
}

MGEngine::Domain MGEngine::Canvas::GetDomain() {
    MGEngine::Point2i Pos(0, 0);
    MGEngine::Point2i Siz = this->GetSize();

    return MGEngine::Domain(Pos, Siz);
}

MGEngine::Point2i MGEngine::Canvas::GetCursorPositon(bool UsePixelPos) {

    MGEngine::Point2i temp = this->MyCurrentCursorPos;

    if (UsePixelPos) temp.x /= 2;

    return this->MyCurrentCursorPos;
}

MGEngine::Point2i MGEngine::Canvas::GetMousePosition() {
    MGEngine::Point2i MousePos = this->getRealMousePos();

    MousePos.x /= 16;
    MousePos.y /= 16;

    if (!MGEngine::IsPointInDomain(MousePos, this->GetDomain())) MousePos.Init(-1, -1);

    return MousePos;
}

bool MGEngine::Canvas::GetMouseLeftClickState() {
    return GetKeyState(VK_LBUTTON) < 0;
}

bool MGEngine::Canvas::GetMouseRightClickState() {
    return GetKeyState(VK_RBUTTON) < 0;
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const char C) {
    if (this->MyCurrentCursorPos.x >= this->MyRealSize.x || this->MyCurrentCursorPos.y >= this->MyRealSize.y) {
        return *this;
    }
    if (this->MyCurrentCursorPos.x < 0 || this->MyCurrentCursorPos.y < 0) {
        return *this;
    }

    this->checkCursorPos();

    if (C == '\n') {
        this->MyCurrentCursorPos.x = 0;
        this->MyCurrentCursorPos.y++;
        return *this;
    }


    int index = this->indexOf(this->MyCurrentCursorPos);
    this->MyCanvasArray[index].Char.UnicodeChar = C;
    this->MyFontColorArray[index] = this->MyFontColor;

    this->NeedsUpdating = true;
    this->MyCurrentCursorPos.Offset(1, 0);

    return *this;
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const std::string Val) {
    for (int i = 0; i < Val.size(); ++i) {
        this->operator<<(Val[i]);
    }
    return *this;
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const char* Val) {
    return this->operator<<(std::string(Val));
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const bool Val) {
    std::stringstream ss;
    ss << Val;
    return this->operator<<(ss.str());
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const int Val) {
    std::stringstream ss;
    ss << Val;
    return this->operator<<(ss.str());
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const float Val) {
    std::stringstream ss;
    ss << Val;
    return this->operator<<(ss.str());
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const double Val) {
    std::stringstream ss;
    ss << Val;
    return this->operator<<(ss.str());
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const MGEngine::Color Val) {
    this->SetFontColor(Val.IgnoreMe);
    return *this;
}

MGEngine::Canvas& MGEngine::Canvas::operator<<(const MGEngine::Point2i Val) {
    this->SetCursorPosition(Val);
    return *this;
}

void MGEngine::Canvas::operator<<(const MGEngine::RawScreenData RSD) {
    if (this->MyRealSize == RSD.MatrixSize) {
        this->MyCanvasArray = RSD.CanvasMatrix;
        this->MyColorArray = RSD.ColorMatrix;
        this->MyFontColorArray = RSD.FontColorMatrix;

        this->NeedsUpdating = true;
    }
}

bool MGEngine::Canvas::operator>>(std::string& Val) {
    Val = this->loadUserInput();
    return true;    //lol ok
}

bool MGEngine::Canvas::operator>>(bool& Val) {
    std::stringstream ss(this->loadUserInput());
    if (ss >> Val) return true;
    return false;
}

bool MGEngine::Canvas::operator>>(int& Val) {
    std::stringstream ss(this->loadUserInput());
    if (ss >> Val) return true;
    return false;
}

bool MGEngine::Canvas::operator>>(char& Val) {
    std::stringstream ss(this->loadUserInput());
    if (ss >> Val) return true;
    return false;
}

bool MGEngine::Canvas::operator>>(float& Val) {
    std::stringstream ss(this->loadUserInput());
    if (ss >> Val) return true;
    return false;
}

bool MGEngine::Canvas::operator>>(double& Val) {
    std::stringstream ss(this->loadUserInput());
    if (ss >> Val) return true;
    return false;
}

bool MGEngine::Canvas::operator>>(RawScreenData& Val) {
    Val.CanvasMatrix = this->MyCanvasArray;
    Val.ColorMatrix = this->MyColorArray;
    Val.FontColorMatrix = this->MyFontColorArray;
    Val.MatrixSize = this->MyRealSize;
    return true;
}

void MGEngine::Canvas::ClearCanvas(uint Color) {

    for (int y = 0; y < this->MyRealSize.y; ++y) {
        for (int x = 0; x < this->MyRealSize.x; ++x) {
            int index = this->indexOf(x, y);

            this->MyCanvasArray[index].Char.UnicodeChar = IgnoreChar;
            this->MyColorArray[index] = Color;
            this->MyFontColorArray[index] = this->MyFontColor;

        }

        this->SetCursorPosition(0, 0);
        this->NeedsUpdating = true;
    }

}

void MGEngine::Canvas::Draw() {
    if (this->NeedsUpdating) {
        this->updateCanvasColors();
        this->NeedsUpdating = false;
    }

    COORD BufferSize;
    BufferSize.X = this->MyRealSize.x;
    BufferSize.Y = this->MyRealSize.y;

    COORD BuffCoord{ 0,0 };

    SMALL_RECT SR;

    SR.Top = 0;
    SR.Left = 0;
    SR.Right = this->MyRealSize.x;
    SR.Bottom = this->MyRealSize.y;

    WriteConsoleOutput(this->MyOutput, &this->MyCanvasArray[0], BufferSize, BuffCoord, &SR);
}

MGEngine::Color MGEngine::SetFontColor(uint ColorVal) {
    MGEngine::Color C;
    C.IgnoreMe = ColorVal;
    return C;
}

void MGEngine::PlaceTextBlock(MGEngine::Canvas& MyCan, std::string Text, MGEngine::uint Color, int xPos, int yPos) {
    MGEngine::PlaceTextBlock(MyCan, Text, Color, MGEngine::Point2i(xPos, yPos));
}

void MGEngine::PlaceTextBlock(MGEngine::Canvas& MyCan, std::string Text, MGEngine::uint Color, MGEngine::Point2i Pos) {

    if (Pos.x < 0 || Pos.x >= MyCan.GetSize().x * 2) return;
    if (Pos.y < 0 || Pos.y >= MyCan.GetSize().y) return;

    MGEngine::Point2i OriginalPosition = MyCan.GetCursorPositon(false);
    MGEngine::uint OriginalColor = MyCan.GetFontColor();

    MGEngine::Point2i MyCursorPosition = Pos;
    MyCursorPosition.x *= 2;

    MyCan.SetCursorPosition(MyCursorPosition, false);
    MyCan.SetFontColor(Color);

    for (int i = 0; i < Text.size(); ++i) {

        if (Text[i] == '\n') {
            MyCursorPosition.Offset(0, 1);
            MyCan.SetCursorPosition(MyCursorPosition, false);
        }
        else {
            MyCan << Text[i];
        }

    }

    MyCan.SetCursorPosition(OriginalPosition, false);
    MyCan.SetFontColor(OriginalColor);
}

void MGEngine::AppendLine(MGEngine::Canvas& MyCan, uint LineCol, int x1, int y1, int x2, int y2) {
    int xs = min(x1, x2);
    int ys = min(y1, y2);

    int xe = max(x1, x2);
    int ye = max(y1, y2);

    if (x2 - x1 == 0) {
        for (int Y = ys; Y <= ye; ++Y) {
            MyCan.SetPixel(LineCol, x1, Y);
        }
        return;
    }

    double K = (double)(y2 - y1) / (double)(x2 - x1);

    for (int Y = ys; Y <= ye; ++Y) {
        double X = -((y1 - K * x1 - Y) / K);

        MyCan.SetPixel(LineCol, std::round(X), Y);
    }

    for (int X = xs; X <= xe; ++X) {
        double Y = K * (X - x1) + y1;

        MyCan.SetPixel(LineCol, X, std::round(Y));
    }
}

void MGEngine::AppendLine(MGEngine::Canvas& MyCan, uint LineCol, MGEngine::Point2i P1, MGEngine::Point2i P2) {
    MGEngine::AppendLine(MyCan, LineCol, P1.x, P1.y, P2.x, P2.y);
}

MGEngine::GenericShape::GenericShape(MGEngine::Canvas* Can) {
    this->MyCanvas = Can;
}

MGEngine::GenericShape::GenericShape(MGEngine::Canvas* Can, MGEngine::Domain D) : GenericShape(Can) {
    this->MyDomain = D;
}

MGEngine::GenericShape::GenericShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize) : GenericShape(Can, MGEngine::Domain(xPos, yPos, xSize, ySize)) {}

MGEngine::GenericShape::GenericShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size) : GenericShape(Can, MGEngine::Domain(Pos, Size)) {}

MGEngine::GenericShape& MGEngine::GenericShape::SetSize(int xSize, int ySize) {
    return this->SetSize(MGEngine::Point2i(xSize, ySize));
}

MGEngine::GenericShape& MGEngine::GenericShape::SetSize(MGEngine::Point2i Size) {
    this->MyDomain.Size = Size;
    return *this;
}

MGEngine::GenericShape& MGEngine::GenericShape::SetPosition(int xPos, int yPos) {
    return this->SetPosition(MGEngine::Point2i(xPos, yPos));
}

MGEngine::GenericShape& MGEngine::GenericShape::SetPosition(MGEngine::Point2i Pos) {
    this->MyDomain.Position = Pos;
    return *this;
}

MGEngine::GenericShape& MGEngine::GenericShape::SetFillColor(uint Color) {
    this->SetOutlineColor(Color);
    this->SetInnerColor(Color);
    return *this;
}

MGEngine::GenericShape& MGEngine::GenericShape::SetOutlineColor(uint Color) {
    this->MyColorOuter = Color;
    return *this;
}

MGEngine::GenericShape& MGEngine::GenericShape::SetInnerColor(uint Color) {
    this->MyColorInner = Color;
    return *this;
}

MGEngine::GenericShape& MGEngine::GenericShape::SetFillMode(bool Fill) {
    this->MyFillMode = Fill;
    return *this;
}

MGEngine::Point2i MGEngine::GenericShape::GetPosition() {
    return this->MyDomain.Position;
}

MGEngine::Point2i MGEngine::GenericShape::GetSize() {
    return this->MyDomain.Size;
}

MGEngine::Domain MGEngine::GenericShape::GetDomain() {
    return this->MyDomain;
}

MGEngine::uint MGEngine::GenericShape::GetOuterColor() {
    return this->MyColorOuter;
}

MGEngine::uint MGEngine::GenericShape::GetInnerColor() {
    return this->MyColorInner;
}

MGEngine::BoxShape::BoxShape(MGEngine::Canvas* Can) : GenericShape(Can) {}

MGEngine::BoxShape::BoxShape(MGEngine::Canvas* Can, MGEngine::Domain D) : GenericShape(Can, D) {}

MGEngine::BoxShape::BoxShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize) : GenericShape(Can, xPos, yPos, xSize, ySize) {}

MGEngine::BoxShape::BoxShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size) : GenericShape(Can, Pos, Size) {}

void MGEngine::BoxShape::Append() {

    int xs = this->MyDomain.Position.x;
    int ys = this->MyDomain.Position.y;

    int xe = (this->MyDomain.Position + this->MyDomain.Size).x;
    int ye = (this->MyDomain.Position + this->MyDomain.Size).y;

    for (int y = ys; y <= ye; ++y) {
        for (int x = xs; x <= xe; ++x) {
            uint FillColor;
            if (x == xs || y == ys || x == xe || y == ye) {
                FillColor = this->MyColorOuter;
                this->MyCanvas->SetPixel(FillColor, x, y);
            }
            else if (this->MyFillMode) {
                FillColor = this->MyColorInner;
                this->MyCanvas->SetPixel(FillColor, x, y);
            }

        }
    }
}

MGEngine::CircleShape::CircleShape(MGEngine::Canvas* Can)
    : GenericShape(Can) {}

MGEngine::CircleShape::CircleShape(MGEngine::Canvas* Can, MGEngine::Domain D)
    : GenericShape(Can, D) {}

MGEngine::CircleShape::CircleShape(MGEngine::Canvas* Can, int xPos, int yPos, int xSize, int ySize)
    : GenericShape(Can, xPos, yPos, xSize, ySize) {}

MGEngine::CircleShape::CircleShape(MGEngine::Canvas* Can, MGEngine::Point2i Pos, MGEngine::Point2i Size)
    : GenericShape(Can, Pos, Size) {}

void MGEngine::CircleShape::Append() {
    int x1 = this->MyDomain.Position.x;
    int x2 = (this->MyDomain.Position + this->MyDomain.Size).x;

    int y1 = this->MyDomain.Position.y;
    int y2 = (this->MyDomain.Position + this->MyDomain.Size).y;

    uint LineColor = this->MyColorOuter;

    int xs = min(x1, x2);
    int ys = min(y1, y2);

    int xe = max(x1, x2);
    int ye = max(y1, y2);

    double P = (xs + xe) / 2.0;
    double Q = (ys + ye) / 2.0;

    double A = (xe - xs) / 2.0;
    double B = (ye - ys) / 2.0;

    for (int X = xs; X <= xe; ++X) {
        double y1 = std::sqrt(std::pow(B, 2) - ((std::pow(B, 2) * std::pow(X - P, 2)) / std::pow(A, 2)));
        double y2 = -y1;

        if (this->MyFillMode) MGEngine::AppendLine(*this->MyCanvas, this->MyColorInner, X, std::round(y1 + Q), X, std::round(y2 + Q));

        this->MyCanvas->SetPixel(LineColor, X, std::round(y1 + Q));
        this->MyCanvas->SetPixel(LineColor, X, std::round(y2 + Q));
    }

    for (int Y = ys; Y <= ye; ++Y) {
        double x1 = std::sqrt(std::pow(A, 2) - ((std::pow(A, 2) * std::pow(Y - Q, 2)) / std::pow(B, 2)));
        double x2 = -x1;

        this->MyCanvas->SetPixel(LineColor, std::round(x1 + P), Y);
        this->MyCanvas->SetPixel(LineColor, std::round(x2 + P), Y);
    }
}

void MGEngine::GUI_Button::calcTextOff() {
    this->MyTextOffset = this->MySize.y / 2;

    for (int i = 0; i < this->MyButtonText.size(); ++i) {
        if (this->MyButtonText[i] == '\n') this->MyTextOffset--;
    }
}

MGEngine::GUI_Button::GUI_Button(MGEngine::Canvas* Can) {
    this->MyCanvas = Can;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetSize(int xSize, int ySize) {
    return this->SetSize(MGEngine::Point2i(xSize, ySize));
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetSize(MGEngine::Point2i Size) {
    this->MySize = Size;
    this->NeedCalculatingOffset = true;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetPosition(int xPos, int yPos) {
    return this->SetPosition(MGEngine::Point2i(xPos, yPos));
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetPosition(MGEngine::Point2i Pos) {
    this->MyPos = Pos;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetDomain(MGEngine::Domain Dom) {
    this->SetPosition(Dom.Position);
    this->SetSize(Dom.Size);
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetDomain(int xPos, int yPos, int xSize, int ySize) {
    return this->SetDomain(MGEngine::Domain(xPos, yPos, xSize, ySize));
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetColor(MGEngine::uint Color) {
    this->MyColor = Color;
    this->MySelectedColor = Color;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetSelectedColor(MGEngine::uint Color) {
    this->MySelectedColor = Color;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::SetTextColor(MGEngine::uint Color) {
    this->MyTextColor = Color;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(std::string str) {
    this->MyButtonText += str;
    this->NeedCalculatingOffset = true;
    return *this;
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(const char* str) {
    return this->operator<<(std::string(str));
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(bool str) {
    std::stringstream ss;
    ss << str;
    return this->operator<<(ss.str());
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(int str) {
    std::stringstream ss;
    ss << str;
    return this->operator<<(ss.str());
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(char str) {
    std::stringstream ss;
    ss << str;
    return this->operator<<(ss.str());
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(float str) {
    std::stringstream ss;
    ss << str;
    return this->operator<<(ss.str());
}

MGEngine::GUI_Button& MGEngine::GUI_Button::operator<<(double str) {
    std::stringstream ss;
    ss << str;
    return this->operator<<(ss.str());
}

MGEngine::Point2i MGEngine::GUI_Button::GetSize() {
    return this->MySize;
}

MGEngine::Point2i MGEngine::GUI_Button::GetPosition() {
    return this->MyPos;
}

MGEngine::Domain MGEngine::GUI_Button::GetDomain() {
    return MGEngine::Domain(this->GetPosition(), this->GetSize().Offset(-1, -1));
}

MGEngine::uint MGEngine::GUI_Button::GetColor() {
    return this->MyColor;
}

MGEngine::uint MGEngine::GUI_Button::GetSelectedColor() {
    return this->MySelectedColor;
}

MGEngine::uint MGEngine::GUI_Button::GetTextColor() {
    return this->MyTextColor;
}

bool MGEngine::GUI_Button::IsHovered() {
    return MGEngine::IsPointInDomain(this->MyCanvas->GetMousePosition(), this->GetDomain());
}

bool MGEngine::GUI_Button::IsPressed() {
    return (this->IsHovered() && this->MyCanvas->GetMouseLeftClickState());
}

void MGEngine::GUI_Button::ClearText() {
    this->MyButtonText.clear();
    this->NeedCalculatingOffset = true;
}

void MGEngine::GUI_Button::Append() {

    if (this->NeedCalculatingOffset) {
        this->calcTextOff();
        this->NeedCalculatingOffset = false;
    }

    int xs = this->GetPosition().x;
    int ys = this->GetPosition().y;

    int xe = (this->GetPosition() + this->GetSize()).x;
    int ye = (this->GetPosition() + this->GetSize()).y;

    MGEngine::uint Color;
    if (this->IsHovered()) Color = this->MySelectedColor;
    else Color = this->MyColor;

    for (int y = ys; y < ye; ++y) {
        for (int x = xs; x < xe; ++x) {
            this->MyCanvas->SetPixel(Color, x, y);
        }
    }

    MGEngine::PlaceTextBlock(*this->MyCanvas, this->MyButtonText, this->MyTextColor, this->GetPosition().x, this->GetPosition().y + this->MyTextOffset);
}

void MGEngine::GUI_ButtonList::setupAllTheButtons() {

    MGEngine::Point2i PlaceButtonHere = this->MyPos;

    MGEngine::Point2i Offset = this->MySize;
    Offset.x += this->MySpaceBetweenButtons;
    Offset.y += this->MySpaceBetweenButtons;

    Offset.x *= this->MyNextButtonPos.x;
    Offset.y *= this->MyNextButtonPos.y;

    for (auto& el : this->MyButtonArray) {

        el.SetPosition(PlaceButtonHere);
        el.SetSize(this->MySize);
        el.SetColor(this->MyColor);
        el.SetSelectedColor(this->MySelectedColor);
        el.SetTextColor(this->MyTextColor);

        PlaceButtonHere += Offset;
    }

    this->MyButtonsWereChanged = false;
}

MGEngine::GUI_ButtonList::GUI_ButtonList(MGEngine::Canvas* Can) {
    this->MyCanvas = Can;
    this->MyNextButtonPos.Init(0, 1);
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetButtonSize(int xSize, int ySize) {
    return this->SetButtonSize(MGEngine::Point2i(xSize, ySize));
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetButtonSize(MGEngine::Point2i Size) {
    this->MySize = Size;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetListOrigin(int xPos, int yPos) {
    return this->SetListOrigin(MGEngine::Point2i(xPos, yPos));
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetListOrigin(MGEngine::Point2i Pos) {
    this->MyPos = Pos;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetColor(MGEngine::uint Color) {
    this->MyColor = Color;
    this->MySelectedColor = Color;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetTextColor(MGEngine::uint Color) {
    this->MyTextColor = Color;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetSelectedColor(MGEngine::uint Color) {
    this->MySelectedColor = Color;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetSpaceBetweenButtons(int Val) {
    this->MySpaceBetweenButtons = Val;
    this->MyButtonsWereChanged = true;
    return *this;
}

MGEngine::GUI_ButtonList& MGEngine::GUI_ButtonList::SetButtonPlacementOrder(int x, int y) {
    if (!MGEngine::Point2i(0, 0).Compare(x, y)) {
        this->MyNextButtonPos.Init(x, y);
        this->MyButtonsWereChanged = true;
    }
    return *this;
}

void MGEngine::GUI_ButtonList::AddEmptyButton() {
    this->MyButtonArray.emplace_back(this->MyCanvas);
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::RemoveButton(MGEngine::uint index) {
    if (index >= this->GetButtonCount()) return;

    this->MyButtonArray.erase(this->MyButtonArray.begin() + index);
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, std::string str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const char* str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const bool str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const int str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const char str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const float str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::AddTextToButton(MGEngine::uint ButtonIndex, const double str) {
    this->MyButtonArray[ButtonIndex] << str;
    this->MyButtonsWereChanged = true;
}

void MGEngine::GUI_ButtonList::operator<<(std::string str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const char* str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const bool str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const int str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const char str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const float str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

void MGEngine::GUI_ButtonList::operator<<(const double str) {
    this->AddEmptyButton();
    this->AddTextToButton(this->GetButtonCount() - 1, str);
}

bool MGEngine::GUI_ButtonList::operator>>(int& Selected) {
    return this->GetSelectedButton(&Selected);
}

bool MGEngine::GUI_ButtonList::GetSelectedButton(int* ButtonIndex) {

    if (this->MyCanvas->GetMouseLeftClickState() && this->GetHoveredButton(ButtonIndex)) {
        return true;
    }
    else {
        *ButtonIndex = -1;
        return false;
    }

}

bool MGEngine::GUI_ButtonList::GetHoveredButton(int* ButtonIndex) {
    *ButtonIndex = -1;

    for (int i = 0; i < this->GetButtonCount(); ++i) {
        if (this->MyButtonArray[i].IsHovered()) {
            *ButtonIndex = i;
            return true;
        }
    }

    return false;
}

MGEngine::Point2i MGEngine::GUI_ButtonList::GetListOrigin() {
    return this->MyPos;
}

MGEngine::Domain MGEngine::GUI_ButtonList::GetListDomain() {

    MGEngine::Domain TempDom;
    TempDom.Position = this->GetListOrigin();

    MGEngine::Point2i Size = this->MySize;
    if (this->MyNextButtonPos.x != 0) {
        Size.x += this->MySpaceBetweenButtons;
        Size.x *= this->GetButtonCount();
    }
    if (this->MyNextButtonPos.y != 0) {
        Size.y += this->MySpaceBetweenButtons;
        Size.y *= this->GetButtonCount();
    }

    TempDom.Size = Size;

    return TempDom;
}

MGEngine::Point2i MGEngine::GUI_ButtonList::GetButtonSize() {
    return this->MySize;
}

MGEngine::Domain MGEngine::GUI_ButtonList::GetButtonDomain(MGEngine::uint ButtonIndex) {
    return this->MyButtonArray[ButtonIndex].GetDomain();
}

MGEngine::uint MGEngine::GUI_ButtonList::GetColor() {
    return this->MyColor;
}

MGEngine::uint MGEngine::GUI_ButtonList::GetTextColor() {
    return this->MyTextColor;
}

MGEngine::uint MGEngine::GUI_ButtonList::GetSelectedColor() {
    return this->MySelectedColor;
}

MGEngine::uint MGEngine::GUI_ButtonList::GetButtonCount() {
    return this->MyButtonArray.size();
}

int MGEngine::GUI_ButtonList::GetSpaceBetweenButtons() {
    return this->MySpaceBetweenButtons;
}

void MGEngine::GUI_ButtonList::ClearButtonText(MGEngine::uint ButtonIndex) {
    this->MyButtonArray[ButtonIndex].ClearText();
}

void MGEngine::GUI_ButtonList::DestroyAllButtons() {
    this->MyButtonArray.clear();
}

void MGEngine::GUI_ButtonList::Append() {
    if (this->MyButtonsWereChanged) this->setupAllTheButtons();

    for (auto& el : this->MyButtonArray) {
        el.Append();
    }

}

int MGEngine::Sprite::indexOf(int x, int y) {
    return y * this->MySize.x + x;
}

int MGEngine::Sprite::indexOf(MGEngine::Point2i P) {
    return this->indexOf(P.x, P.y);
}

void MGEngine::Sprite::resizeVector() {
    this->MyColorArray.clear();
    this->MyColorArray.resize(this->MySize.x * this->MySize.y);
}

MGEngine::Sprite::Sprite(MGEngine::Canvas* Can) {
    this->MyCan = Can;
}

MGEngine::Sprite::Sprite(MGEngine::Canvas* Can, int xSize, int ySize)
    : Sprite(Can, MGEngine::Point2i(xSize, ySize)) {}

MGEngine::Sprite::Sprite(MGEngine::Canvas* Can, MGEngine::Point2i Size)
    : Sprite(Can) {
    this->SetSize(Size);
    this->resizeVector();
}

MGEngine::Sprite& MGEngine::Sprite::SetSize(int xSize, int ySize) {
    return this->SetSize(MGEngine::Point2i(xSize, ySize));
}

MGEngine::Sprite& MGEngine::Sprite::SetSize(MGEngine::Point2i Size) {
    this->MySize = Size;
    this->resizeVector();
    return *this;
}

MGEngine::Sprite& MGEngine::Sprite::SetPosition(int xPos, int yPos) {
    return this->SetPosition(MGEngine::Point2i(xPos, yPos));
}

MGEngine::Sprite& MGEngine::Sprite::SetPosition(MGEngine::Point2i Pos) {
    this->MyPos = Pos;
    return*this;
}

MGEngine::Sprite& MGEngine::Sprite::SetPixel(MGEngine::uint Color, int x, int y) {
    if (x < 0 || x >= this->GetSize().x) return *this;
    if (y < 0 || y >= this->GetSize().y) return *this;

    this->MyColorArray[this->indexOf(x, y)] = Color;

    return *this;
}

MGEngine::Sprite& MGEngine::Sprite::SetPixel(MGEngine::uint Color, MGEngine::Point2i P) {
    return this->SetPixel(Color, P.x, P.y);
}

MGEngine::Sprite& MGEngine::Sprite::SetIgnoredColor(MGEngine::uint Color) {
    this->MyIgnoredColor = Color;
    return *this;
}

MGEngine::Point2i MGEngine::Sprite::GetSize() {
    return this->MySize;
}

MGEngine::Point2i MGEngine::Sprite::GetPosition() {
    return this->MyPos;
}

MGEngine::Domain MGEngine::Sprite::GetDomain() {
    return MGEngine::Domain(this->GetPosition(), this->GetSize());
}

MGEngine::uint MGEngine::Sprite::GetPixel(int x, int y) {
    if (x < 0 || x >= this->GetSize().x) return 0;
    if (y < 0 || y >= this->GetSize().y) return 0;

    return this->MyColorArray[this->indexOf(x, y)];
}

MGEngine::uint MGEngine::Sprite::GetPixel(MGEngine::Point2i P) {
    return this->GetPixel(P.x, P.y);
}

MGEngine::uint MGEngine::Sprite::GetIgnoredColor() {
    return this->MyIgnoredColor;
}

void MGEngine::Sprite::LoadFromFile(std::string FileName) {
    std::string name = "./Sprites/";
    name += FileName;
    name += ".MGSprite";

    std::ifstream input(name, std::ios_base::binary);
    if (!input) {
        std::cerr << "Failed to load sprite!";
        exit(1);
    }

    input.read((char*)&this->MySize, sizeof(MGEngine::Point2i));
    this->resizeVector();

    int total = this->MySize.x * this->MySize.y;
    MGEngine::uint* tfield = new MGEngine::uint[total];

    input.read((char*)tfield, sizeof(MGEngine::uint) * total);

    for (int i = 0; i < total; ++i) {
        this->MyColorArray[i] = tfield[i];
    }

    delete[] tfield;
    input.close();
}

void MGEngine::Sprite::SaveToFile(std::string FileName) {
    std::string name = "./Sprites/";
    name += FileName;
    name += ".MGSprite";

    std::ofstream output(name, std::ios_base::binary);
    if (!output) {
        std::cerr << "Failed to save sprite!";
        exit(1);
    }

    output.write((char*)&this->MySize, sizeof(MGEngine::Point2i));

    int total = this->MySize.x * this->MySize.y;
    MGEngine::uint* tfield = new MGEngine::uint[total];

    for (int i = 0; i < total; ++i) tfield[i] = this->MyColorArray[i];

    output.write((char*)tfield, sizeof(MGEngine::uint) * total);

    delete[] tfield;
    output.close();
}

void MGEngine::Sprite::ClearSprite() {
    this->resizeVector();
}

void MGEngine::Sprite::Append(bool UseIgnoreColor) {

    for (int y = 0; y < this->GetSize().y; ++y) {
        for (int x = 0; x < this->GetSize().x; ++x) {
            MGEngine::Point2i t(x, y);

            MGEngine::uint C = this->GetPixel(t);

            if (C == this->MyIgnoredColor && UseIgnoreColor) continue;

            this->MyCan->SetPixel(C, this->MyPos + t);
        }
    }

}

MGEngine::Animation::Animation(MGEngine::Canvas* Can, std::string FileName) {
    this->MyCan = Can;
    this->LoadFromFile(FileName);
}

MGEngine::Animation::Animation(MGEngine::Canvas* Can, int xSize, int ySize)
    : Animation(Can, MGEngine::Point2i(xSize, ySize)) {}

MGEngine::Animation::Animation(MGEngine::Canvas* Can, MGEngine::Point2i Size) {
    this->MyCan = Can;
    this->MySize = Size;
}

MGEngine::Animation& MGEngine::Animation::SetPosition(int xPos, int yPos) {
    return this->SetPosition(MGEngine::Point2i(xPos, yPos));
}

MGEngine::Animation& MGEngine::Animation::SetPosition(MGEngine::Point2i Pos) {
    this->MyPos = Pos;

    return *this;
}

void MGEngine::Animation::AddNewFrame() {
    this->MySpriteArray.emplace_back(this->MyCan, this->MySize);
}

void MGEngine::Animation::RemoveFrame(MGEngine::uint FrameIndex) {
    if (FrameIndex >= this->GetFrameCount()) return;
    this->MySpriteArray.erase(this->MySpriteArray.begin() + FrameIndex);
}

void MGEngine::Animation::RemoveLast() {
    this->MySpriteArray.pop_back();
}

void MGEngine::Animation::SetFrame(MGEngine::uint FrameIndex, MGEngine::Sprite& spr) {
    if (FrameIndex >= this->GetFrameCount()) return;

    this->MySpriteArray[FrameIndex] = spr;
}

void MGEngine::Animation::SetPixel(MGEngine::uint FrameIndex, MGEngine::uint Color, int x, int y) {
    this->SetPixel(FrameIndex, Color, MGEngine::Point2i(x, y));
}

void MGEngine::Animation::SetPixel(MGEngine::uint FrameIndex, MGEngine::uint Color, MGEngine::Point2i P) {
    if (FrameIndex >= this->GetFrameCount()) return;

    this->MySpriteArray[FrameIndex].SetPixel(Color, P);
}

MGEngine::uint MGEngine::Animation::GetPixel(MGEngine::uint FrameIndex, int x, int y) {
    return this->GetPixel(FrameIndex, MGEngine::Point2i(x, y));
}

MGEngine::uint MGEngine::Animation::GetPixel(MGEngine::uint FrameIndex, MGEngine::Point2i P) {

    return this->MySpriteArray[FrameIndex].GetPixel(P);
}

int MGEngine::Animation::GetFrameCount() {
    return this->MySpriteArray.size();
}

MGEngine::Point2i MGEngine::Animation::GetSize() {
    return this->MySize;
}

MGEngine::Point2i MGEngine::Animation::GetPosition() {
    return this->MyPos;
}

MGEngine::Domain MGEngine::Animation::GetDomain() {
    return MGEngine::Domain(this->MyPos, this->MySize);
}

void MGEngine::Animation::SetActiveFrame(MGEngine::uint FrameIndex) {
    if (FrameIndex >= this->GetFrameCount()) FrameIndex = 0;

    this->CurrentFrame = FrameIndex;

}

void MGEngine::Animation::NextFrame() {
    this->CurrentFrame++;
    if (this->CurrentFrame >= this->GetFrameCount()) this->CurrentFrame = 0;
}

void MGEngine::Animation::LastFrame() {
    if (this->CurrentFrame == 0) this->CurrentFrame = this->GetFrameCount();
    this->CurrentFrame--;
}

void MGEngine::Animation::LoadFromFile(std::string FileName) {

    std::string name = "./Animations/";
    name += FileName;
    name += ".MGAnimation";

    std::ifstream input(name, std::ios_base::binary);

    if (!input) {
        std::cerr << "Failed to load animation!";
        exit(1);
    }

    int FrameCount;
    input.read((char*)&FrameCount, sizeof(int));
    input.read((char*)&this->MySize, sizeof(MGEngine::Point2i));

    int Total = this->MySize.x * this->MySize.y;
    MGEngine::uint* tfield = new MGEngine::uint[Total];

    for (int i = 0; i < FrameCount; ++i) {
        input.read((char*)tfield, sizeof(MGEngine::uint) * Total);

        int tindex = 0;
        this->AddNewFrame();
        for (int y = 0; y < this->MySize.y; ++y) {
            for (int x = 0; x < this->MySize.x; ++x) {
                this->MySpriteArray[i].SetPixel(tfield[tindex++], x, y);
            }
        }
    }

    input.close();

}

void MGEngine::Animation::SaveToFile(std::string FileName) {

    std::string name = "./Animations/";
    name += FileName;
    name += ".MGAnimation";

    std::ofstream output(name, std::ios_base::binary);

    if (!output) {
        std::cerr << "Failed to save Animation!";
        exit(1);
    }

    int FrameCount = this->MySpriteArray.size();
    output.write((char*)&FrameCount, sizeof(int));
    output.write((char*)&this->MySize, sizeof(MGEngine::Point2i));

    int Total = this->MySize.x * this->MySize.y;
    MGEngine::uint* tfield = new MGEngine::uint[Total];

    for (int i = 0; i < FrameCount; ++i) {
        int tindex = 0;

        for (int y = 0; y < this->MySize.y; ++y) {
            for (int x = 0; x < this->MySize.x; ++x) {
                tfield[tindex++] = this->MySpriteArray[i].GetPixel(x, y);
            }
        }

        output.write((char*)tfield, sizeof(MGEngine::uint) * Total);
    }

    delete[] tfield;
    output.close();
}

void MGEngine::Animation::Append() {
    this->MySpriteArray[this->CurrentFrame].SetPosition(this->MyPos);
    this->MySpriteArray[this->CurrentFrame].Append(false);
}
