/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#ifdef UILIB_EXPORTS
    #define UILIB_API __declspec(dllexport)
#else
    #define UILIB_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251)
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)

//STL Headers
#include <map>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>
#include <random>
#include <initializer_list>

//Boost Headers
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

//Headers from external libraries
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <maximilian.h>
#include <RtAudio.h>

std::vector<std::string> split(const std::string& src, std::string separate_character);
void trim(std::string& s);
std::string strtolower(const std::string& s);

namespace UI
{
    namespace Math
    {
        class UILIB_API Vec2
        {
        public:
            double x, y;
            Vec2() = default;
            Vec2(double _x, double _y);
            Vec2(const Vec2& from, const Vec2& to);
            bool operator == (const Vec2& v) const;
            bool operator != (const Vec2& v) const;
            Vec2 operator + (const Vec2& v) const;
            Vec2 operator - (const Vec2& v) const;
            Vec2 operator * (const double d) const;
            Vec2& set(const Vec2& to);
            Vec2& set(const double _x, const double _y);
        };

        class UILIB_API Vec3
        {
        public:
            double x, y, z;
            Vec3() = default;
            Vec3(double _x, double _y, double _z);
            Vec3(const Vec3& from, const Vec3& to);
            Vec3 operator - (const Vec3& b) const;
            Vec3 operator + (const Vec3& b) const;
            Vec3 operator * (const double b) const;
            Vec3 operator * (const Vec3& b) const; //Cross Product
            Vec3& operator -= (const Vec3& b);
            Vec3& operator += (const Vec3& b);
            Vec3& operator *= (const double b);
            Vec3& operator *= (const Vec3& b); //Cross Product
            Vec3& zero();
            Vec3& set(const Vec3& to);
            Vec3& set(const double _x, const double _y, const double _z);
            double length() const;
            Vec3 unit() const;
            double dot(const Vec3& b) const;
        };

        class UILIB_API Vec4
        {
        public:
            double x, y, z, t;
            Vec4() = default;
            Vec4(double _x, double _y, double _z, double _t);
            Vec4(const Vec4& from, const Vec4& to);
            Vec4 operator - (const Vec4& b) const;
            Vec4 operator + (const Vec4& b) const;
            Vec4 operator * (const double b) const;
            Vec4& operator -= (const Vec4& b);
            Vec4& operator += (const Vec4& b);
            Vec4& operator *= (const double b);
            Vec4& zero();
            Vec4& set(const Vec4& to);
            Vec4& set(const double _x, const double _y, const double _z, const double _t);
            double length() const;
            Vec4 unit() const;
            double dot(const Vec4& b) const;
        };
    }
    namespace Core
    {
        extern SDL_Renderer* currenderer;
    }
    namespace Base
    {
        using Color = Math::Vec4;
        UILIB_API Color blend(const Color& lhs, const Color& rhs);
        class UILIB_API Rect
        {
        public:
            double xmin, xmax, ymin, ymax;
            Rect() = default;
            Rect(Math::Vec2 p1, Math::Vec2 p2);
            Rect(double _xmin, double _xmax, double _ymin, double _ymax);
            void set(double _xmin, double _xmax, double _ymin, double _ymax);
        };

        class UILIB_API Texture
        {
        public:
            Texture() = default;
            Texture(std::string path);
            ~Texture();
        private:
            SDL_Texture * texture = nullptr;
        };

        class UILIB_API Image
        {
        public:
            Image();
            Image(Image& img);
            Image(std::string path);
            Image Sub(Rect range);
            ~Image();
        private:
            std::shared_ptr<Texture> tex;
        };

        class UILIB_API Brush
        {
        public:
            virtual Color sample(double x, double y) = 0;
            virtual ~Brush() = default;
        };

        UILIB_API void init();
    }

    namespace Logger
    {
        enum class Level : size_t
        {
            trace, debug, info, warning, error, fatal
        };

        class UILIB_API Logger
        {
        public:
            Logger() = default;
            Logger(const std::string& path);
            Logger(const std::string& path, Level _clogLevel, Level _cerrLevel, Level _fileLevel, Level _lineLevel);

            void log(Level level, const std::string& message, const char* fileName, int lineNumber);
            void dump();

            template <typename T>
            Logger& operator<< (const T& rhs)
            {
                m_content << rhs;
                return *this;
            }
        private:
            Level clogLevel, cerrLevel, fileLevel, lineLevel;
            std::string logpath;
            std::stringstream m_content;
        };

        UILIB_API extern Logger service;
        UILIB_API void init(const std::string& path);

#define logdebug(x)    UI::Logger::service.log(UI::Logger::Level::debug  , x, __FUNCTION__, __LINE__)
#define loginfo(x)     UI::Logger::service.log(UI::Logger::Level::info   , x, __FUNCTION__, __LINE__)
#define logwarning(x)  UI::Logger::service.log(UI::Logger::Level::warning, x, __FUNCTION__, __LINE__)
#define logerror(x)    UI::Logger::service.log(UI::Logger::Level::error  , x, __FUNCTION__, __LINE__)
#define logfatal(x)    UI::Logger::service.log(UI::Logger::Level::fatal  , x, __FUNCTION__, __LINE__)
    }

    namespace Data
    {
        //Some Basic Convertions with String
        UILIB_API bool isDecimal(const std::string& str);
        UILIB_API bool isInteger(const std::string& str);
        UILIB_API bool isBoolean(const std::string& str);
        UILIB_API bool isString(const std::string& str);
        UILIB_API double getDecimal(const std::string& str);
        UILIB_API int getInteger(const std::string& str);
        UILIB_API bool getBoolean(const std::string& str);
        UILIB_API const std::string getString(const std::string& str);

        namespace Convert
        {
            boost::spirit::hold_any StrToAny(const std::string& str);
            UILIB_API std::string ToString(boost::spirit::hold_any var);
        }

        using AnyTMap = std::map<std::string, boost::spirit::hold_any>;
        enum class SimpleMapFileExceptions : size_t
        {
            IOFailureOnRead, IOFailureOnWrite
        };

        class UILIB_API SimpleMapFile
        {
        public:
            SimpleMapFile(const std::string& filename);
            void save();
            const AnyTMap& getMap();
            boost::spirit::hold_any get(const std::string& key) const;
            bool has(const std::string& key) const;
            void set(const std::string& key, const boost::spirit::hold_any value);
            template<class T>
            T get(const std::string& key, T defaultValue = T()) const
            {
                try
                {
                    auto result = m_settings.find(key);
                    return (result != m_settings.end()) ? result->second.cast<T>() : defaultValue;
                }
                catch(boost::spirit::bad_any_cast e)
                {
                    logerror("boost bad cast:" + std::string(e.from) + " to " + std::string(e.to));
                    UI::Logger::service.dump();
                }

                return T();
            }
        private:
            AnyTMap m_settings;
            std::string m_filename;
        };
    }

    namespace Core
    {
        enum class Exceptions : size_t
        {
            WindowCreationFailure,
            VideoInitFailure,
            AudioInitFailure
        };

        enum StatChange
        {
            Minimize, Maximize, Restore
        };

        enum FocusOp
        {
            Lose, Gain
        };

        enum ButtonAction
        {
            Release = 0, Press, Repeat
        };

        enum MouseButton
        {
            Left, Middle, Right, Preserved1, Preserved2
        };

        enum CursorOp
        {
            Leave, Enter
        };

        enum class VerticalAlignment : size_t
        {
            Top, Bottom, Center, Stretch
        };

        enum class HorizontalAlignment : size_t
        {
            Left, Right, Center, Stretch
        };

        class Grid;

        //relativeps: percentage relativepc:position
        class UILIB_API Margin
        {
        public:
            Margin() = default;
            Margin(Base::Rect _relative_ps, Base::Rect _relative_pc, HorizontalAlignment _ha, VerticalAlignment _va);
            void updateAbs(Base::Rect Parent_Rect);
            Base::Rect absrect;
            //Quick Constructors
            inline static auto TopLeft(double TopPct, double LeftPct, double TopDis, double LeftDis, double Width, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, 0.0), Base::Rect(LeftDis, Width, TopDis, Height), HorizontalAlignment::Left, VerticalAlignment::Top);
            }
            inline static auto TopRight(double TopPct, double RightPct, double TopDis, double RightDis, double Width, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, 0.0), Base::Rect(Width, RightDis, TopDis, Height), HorizontalAlignment::Right, VerticalAlignment::Top);
            }
            inline static auto TopCenter(double TopPct, double TopDis, double Height, double CenterLeft, double CenterRight)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, 0.0), Base::Rect(CenterLeft, CenterRight, TopDis, Height), HorizontalAlignment::Center, VerticalAlignment::Top);
            }
            inline static auto TopStretch(double TopPct, double LeftPct, double RightPct, double TopDis, double Height, double LeftDis, double RightDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, 0.0), Base::Rect(LeftDis, RightDis, TopDis, Height), HorizontalAlignment::Stretch, VerticalAlignment::Top);
            }
            inline static auto BottomLeft(double LeftPct, double BottomPct, double LeftDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, BottomPct), Base::Rect(LeftDis, Width, Height, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Bottom);
            }
            inline static auto BottomRight(double RightPct, double BottomPct, double RightDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, BottomPct), Base::Rect(Width, RightDis, Height, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Bottom);
            }
            inline static auto BottomCenter(double BottomPct, double CenterLeft, double CenterRight, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, BottomPct), Base::Rect(CenterLeft, CenterRight, Height, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Bottom);
            }
            inline static auto BottomStretch(double LeftPct, double RightPct, double BottomPct, double  LeftDis, double RightDis, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, BottomPct), Base::Rect(LeftDis, RightDis, Height, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Bottom);
            }
            inline static auto CenterLeft(double LeftPct, double LeftDis, double Width, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, 0.0), Base::Rect(LeftDis, Width, CenterTop, CenterBottom), HorizontalAlignment::Left, VerticalAlignment::Center);
            }
            inline static auto CenterRight(double RightPct, double RightDis, double Width, double CenterTop, double  CenterBottom)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, 0.0), Base::Rect(Width, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Right, VerticalAlignment::Center);
            }
            inline static auto CenterCenter(double CenterLeft, double CenterRight, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, 0.0), Base::Rect(CenterLeft, CenterRight, CenterTop, CenterBottom), HorizontalAlignment::Center, VerticalAlignment::Center);
            }
            inline static auto CenterStretch(double LeftPct, double RightPct, double LeftDis, double RightDis, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, 0.0), Base::Rect(LeftDis, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Stretch, VerticalAlignment::Center);
            }
            inline static auto StretchLeft(double LeftPct, double TopPct, double BottomPct, double LeftDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, BottomPct), Base::Rect(LeftDis, Width, TopDis, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Stretch);
            }
            inline static auto StretchRight(double RightPct, double TopPct, double BottomPct, double RightDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, BottomPct), Base::Rect(Width, RightDis, TopDis, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Stretch);
            }
            inline static auto StretchCenter(double TopPct, double BottomPct, double CenterLeft, double CenterRight, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, BottomPct), Base::Rect(CenterLeft, CenterRight, TopDis, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Stretch);
            }
            inline static auto StretchStretch(double LeftPct, double RightPct, double TopPct, double BottomPct, double LeftDis, double RightDis, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, BottomPct), Base::Rect(LeftDis, RightDis, TopDis, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Stretch);
            }
        private:
            HorizontalAlignment ha;
            VerticalAlignment va;
            Base::Rect relativeps, relativepc;
        };

        class UILIB_API Control
        {
        public:
            std::string xName;  //ID
            Margin cMargin;     //Position Data
            size_t gridPosX, gridPosY;

            Control() = default; //Creator
            Control(std::string _xName, Margin _Margin);

            virtual void setSize(size_t x, size_t y);
            virtual void parentResize(Grid* parent);
            //Event Handlers
            virtual void render();
            virtual void onResize(size_t x, size_t y);
            virtual void onParentResize(Grid* parent);
            virtual void focusFunc(FocusOp Stat);
            virtual void mouseMove(int x, int y, int dx, int dy);
            virtual void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            virtual void crusorEnterFunc(CursorOp Stat);
            virtual void scrollFunc(double dx, double dy);
            virtual void keyFunc(int Key, ButtonAction Action);
            virtual void charInputFunc(std::wstring Char);
            virtual void dropFunc(const char* Paths);
            virtual void touchFunc(int x, int y, ButtonAction action);
            virtual void touchmove(int x, int y, int dx, int dy);

            virtual ~Control() {}
            bool enabled, mouseOn, pressed, focused;
        };

        //CallBack Functions
        using NotifyFunc = std::function<void()>;
        using onFocus = std::function<void(FocusOp)>;
        using onMouseMove = std::function<void(int, int, int, int)>;
        using onMouseButton = std::function<void(MouseButton, ButtonAction)>;
        using onMouseEnter = std::function<void(CursorOp)>;
        using onScroll = std::function<void(double, double)>;
        using onKeyPress = std::function<void(int, ButtonAction)>;
        using onCharInput = std::function<void(wchar_t)>;
        using onFileDrop = std::function<void(const char*)>;
        using onTouch = std::function<void(int, int action)>;
        using onTouchMove = std::function<void(int, int, int, int)>;

        struct Definition
        {
            int min, max, accBegin, accEnd;
        };

        class UILIB_API Grid : public Control
        {
        public:
            std::vector<Definition> colDefinitions;
            std::vector<Definition> rowDefinitions;
            std::map<std::string, std::shared_ptr<Control>> childern;
            Grid();
            void recalGrid();
            void addChild(std::shared_ptr<Control> child);
            void render();
            void setSize(size_t x, size_t y);
            void parentResize(Grid* parent);
            void focusFunc(FocusOp Stat);
            void onResize(size_t x, size_t y);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void charInputFunc(std::wstring Char);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);
        private:
            void recal(std::vector<Definition>& definitions, bool col);
            std::vector<std::shared_ptr<Control>> mold, told;
            std::shared_ptr<Control> focused;
            std::shared_ptr<Control> pressed;
        };

        class UILIB_API Page
        {
        public:
            std::shared_ptr<Grid> content;
        };

        class UILIB_API Window
        {
        public:
            virtual ~Window()
            {
            }

            Window(const std::string& name, const int width, const int height,
                   const int _xpos, const int _ypos);
            void close();
            void resize(size_t x, size_t y);
            virtual void onShow();
            virtual void onHide();
            virtual void onClose();
            virtual void onStatChange(StatChange stat);
            virtual void onResize(size_t x, size_t y);
            //Event Handler
            void render();
            void focusFunc(FocusOp Stat);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void charInputFunc(std::wstring Char);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);

            void pushPage(std::shared_ptr<Page> page);
            void popPage();
            void clearPages();
            void pageZero();

            //SDL ONLY锛?
            Uint32 windowID();
        private:
            std::stack<std::shared_ptr<Page>> pages;
            size_t _x, _y;
            SDL_Window* window;
            SDL_GLContext context;
        };

        class UILIB_API Application
        {
        public:
            virtual ~Application()
            {
            }

            void run();
            void processMessages();
            void addWindow(std::shared_ptr<Window> win);
            void setMainWindow(std::shared_ptr<Window> win);
            void terminate();
            virtual void onTerminate();
            virtual void beforeLaunch();
            virtual void afterLaunch();
            virtual void onEnteringBackground();
            virtual void onEnteringForeground();
        private:
            std::shared_ptr<Window> mainWin, focused;
            std::map<Uint32, std::shared_ptr<Window>> windows;
            void mainLoop();
            bool winExist(int id);
            bool sigExit;
        };
        UILIB_API extern Application* application;
    }

    namespace Graphics
    {
        using namespace Math;
        using namespace Base;
        namespace Shapes
        {
            class UILIB_API Line : public Core::Control
            {
            public:
                Vec2 _p1, _p2;
                std::shared_ptr<Brush> _brush;
                void render();
                Line(const Vec2& p1, Vec2 p2, const std::shared_ptr<Brush> brush);
                static void draw(const Vec2& p1, const Vec2& p2, const std::shared_ptr<Brush> brush);
            };

            class UILIB_API Triangle : public Core::Control
            {
            public:
                Vec2 _p1, _p2, _p3;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Triangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Rectangle : public Core::Control
            {
            public:
                Rect _rect;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Rectangle(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Polygon : public Core::Control
            {
            public:
                std::vector<Vec2> _pts;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Polygon(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Circle : public Core::Control
            {
            public:
                Vec2 _center;
                double _radius;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Circle(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Ellipse : public Core::Control
            {
            public:
                Vec2 _f1, _f2;
                double l;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Ellipse(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };
        }

        namespace Brushes
        {
            class UILIB_API SolidColorBrush : public Brush
            {
            public:
                SolidColorBrush(const Color& _col);
                Color sample(double x, double y);
            private:
                Color col;
            };
#define UIMakeSolidColorBrush(x) std::shared_ptr<Base::Brush>(new Graphics::Brushes::SolidColorBrush(x))

            class UILIB_API GradientBrush : public Brush
            {
            public:
                GradientBrush(const Color& _col0, const Color& _col1, const Color& _col2, const Color& _col3);
                Color sample(double x, double y);
            private:
                Color col0, col1, col2, col3;
            };
        }
    }

    namespace Globalization
    {

        class UILIB_API Str
        {
        public:
            Str(const std::string& str, bool globalizationNeed = true);
            operator std::string() const;
            void flush();

        private:
            const std::string ds; //data string
            std::string gs; //globalization string
        };

        class UILIB_API Service
        {
        public:
            Service() = default;
            void attachLangFiles(std::initializer_list<std::string> filenames);
            const std::string getCurLang() const;
            void setLang(const std::string& _lang);
            const std::string getStr(const std::string& key) const;
            static inline Service& getInstance()
            {
                static Service service;
                return service;
            }
            void setBasePath(std::string _basePath);
            std::string getBasePath();
            void setSuffix(std::string _suffix);
            std::string getSuffix();
        private:
            std::unordered_map<std::string, const Data::SimpleMapFile> langs;
            std::string curLang;
            const Data::SimpleMapFile* curLangData;
            std::string basePath = "langs/", suffix = ".lang";
        };
    }

    namespace Font
    {
        using namespace Base;
        using namespace Core;
        struct UnicodeChar
        {
            unsigned int tex;
            int xpos, ypos;
            int width, height, advance;
            UnicodeChar();
        };

        class UILIB_API FontOfSize // Font with a specific size
        {
        public:
            UnicodeChar GetChar(const wchar_t);
            FontOfSize() = default;
            FontOfSize(const int _height, FT_Face* _face);
            int height;
        private:
            //int refcount;
            FT_Size size;
            FT_Face* fontface;
            FT_GlyphSlot slot;
            std::map<wchar_t, UnicodeChar> chars;
            std::vector<unsigned int> texBuffers;
            int curBufferID, curBufferPos, maxBufferX, maxBufferY;
        };

        class UILIB_API FontRenderer
        {
        public:
            Color color;
            FontRenderer(FontOfSize& _fos, Color col);
            void renderStr(int x, int y, const Rect& r, const std::wstring& str) const;
            void renderStr(int x, int y, const Rect& r, const std::string& str) const;
            double getStrWidth(const std::wstring& str) const;
            double getStrWidth(const std::string& str) const;
            FontOfSize& fos;
        };

        class UILIB_API FontBase
        {
        public:
            std::string path;
            FontBase() = default;
            FontBase(std::string _path);
            std::shared_ptr<FontRenderer> getRenderer(int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<int, FontOfSize> sizes;
            FT_Face fontface;
        };

        class UILIB_API Service
        {
        public:
            Service();
            void addSearchPaths(std::initializer_list<std::string> _apths);
            std::shared_ptr<FontRenderer> getRenderer(const std::string& font, int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<std::string, FontBase> fonts;
            std::vector<std::string> searchpaths;
        };

        UILIB_API extern Service service;
    }

    namespace Controls
    {
        using namespace Base;
        using namespace Core;
        using namespace Font;
        using namespace Globalization;
        ////////////////////////////////////////////////////////////
        //    Basic Control Definitions
        ////////////////////////////////////////////////////////////
        class UILIB_API Label : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<FontRenderer> Font;
            Str caption;
            void render();
            Label() = default;
            Label(std::string _xName, Margin _Margin, Str _caption);
        };

        class UILIB_API Button : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> backgroundOnPressBrush;
            std::shared_ptr<Brush> borderOnPressBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onClick;

            Str caption;
            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            Button() = default;
            Button(std::string _xName, Margin _Margin, Str _caption, NotifyFunc _onClick);
        };

        class UILIB_API TextBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> crusorBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onEditDone;
            std::wstring text;
            Str mask;

            void render();
            void focusFunc(FocusOp Stat);
            void charInputFunc(std::wstring Char);
            void keyFunc(int Key, ButtonAction Action);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            TextBox() = default;
            TextBox(std::string _xName, Margin _Margin, std::wstring _text, Str _mask, NotifyFunc _onEditDone);
        private:
            int left;
            int cursorpxpos = 0;
            size_t cursorpos = 0;
            size_t cposrdcount = 0;
            std::wstring appendingstr;
        };

        enum CheckStat
        {
            Unchecked = 0, Checked, UnKnown
        };

        class UILIB_API CheckBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> checkBrush;
            std::shared_ptr<Brush> checkHeightLightBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onStatChange;

            bool threeStatus;
            CheckStat stat;
            Str caption;

            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            CheckBox() = default;
            CheckBox(std::string _xName, Margin _Margin, Str _caption, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange);
        };

        class UILIB_API GLContext : public Control
        {
        public:
            GLContext() = default;
            GLContext(std::string _xName, Margin _Margin);

            NotifyFunc onRenderF;
            onFocus onFocusF;
            onMouseMove onMouseMoveF;
            onMouseButton onMouseButtonF;
            onMouseEnter onMouseEnterF;
            onScroll onScrollF;
            onKeyPress onKeyPress;
            onFileDrop onFileDropF;
            onTouch onTouchF;
            onTouchMove onTouchMoveF;

            void render();
            void onResize(size_t x, size_t y);
            void onParentResize(Grid* parent);
            void focusFunc(FocusOp Stat);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);
        };
    }

    namespace Theme
    {
        using namespace Base;
        using namespace Core;
        struct Theme
        {
            std::shared_ptr<Brush> ActiveBorderBrush;
            std::shared_ptr<Brush> InactiveBorderBrush;

            std::shared_ptr<Brush> ControlBrush;
            std::shared_ptr<Brush> ControlDarkBrush;
            std::shared_ptr<Brush> ControlDarkDarkBrush;
            std::shared_ptr<Brush> ControlLightBrush;
            std::shared_ptr<Brush> ControlLightLightBrush;

            std::shared_ptr<Brush> ControlHeightLightBrush;
            std::shared_ptr<Brush> ControlOnPressBrush;

            std::shared_ptr<Brush> MenuBrush;
            std::shared_ptr<Brush> MenuBarBrush;
            std::shared_ptr<Brush> MenuHighlightBrush;

            std::shared_ptr<Brush> ScrollBarBrush;
            std::shared_ptr<Brush> WindowBrush;

            std::shared_ptr<Font::FontRenderer> DefaultFont;
        };
        UILIB_API extern Theme SystemTheme;
    }

//#undef UILIB_API
//#define UILIB_API //disiable audio
    namespace Audio
    {
        class UILIB_API Source;

        class UILIB_API Effect
        {
        public:
            Effect() = default;
            virtual ~Effect() = default;
            virtual double operator()(double input, Source* thisSource);
        };

        class UILIB_API EffectEx
        {
        public:

            EffectEx() = default;
            virtual ~EffectEx() = default;
            virtual double operator()(double input, size_t channel, Source* thisSource);
        };

        class UILIB_API Source
        {
        private:
            Math::Vec3 pos;
            double samplePos;
            bool isPlay, loop;
            std::shared_ptr<maxiSample> sample;
            double sampleValue;
        public:
            size_t refcount;
            double freq;//for User
            double realFreq;//for Effects
            double gain;

            Math::Vec3 move;
            vector<shared_ptr<UI::Audio::Effect>> effects;
            vector<shared_ptr<UI::Audio::EffectEx>> effectsExs;

            Source();
            Source(Source& source_);
            Source(std::shared_ptr<maxiSample> sample_);
            ~Source();
            bool isStopped();
            void play(bool loop);
            void stop();
            double setPos(double Pos_ = 0.0);
            double operator()(size_t channel);
            void Sample();
            //operator Math::Vec3();
            //operator Math::Vec3&();
            Math::Vec3& getPos() { return pos; }
        };

        class UILIB_API SourceHandler
        {
        public:
            SourceHandler(Source* src);
            ~SourceHandler();
            Source& get() const;
        private:
            Source* msrc;
        };

        class UILIB_API Listener
        {
        private:
            Math::Vec3 pos;
        public:
            Math::Vec3 move;
            std::vector<Math::Vec3> pos2;

            Listener() = default;
            ~Listener() = default;
            //operator Math::Vec3&();
            Math::Vec3& getPos() { return pos; }
            Math::Vec3 operator[](size_t channel);
        };

        class UILIB_API OutEffect
        {
        public:
            OutEffect() = default;
            virtual ~OutEffect() = default;
            virtual double operator()(double input, size_t channel);
        };

        class UILIB_API OutEffectEx
        {
        public:
            OutEffectEx() = default;
            virtual ~OutEffectEx() = default;
            virtual void operator()(double* input, size_t channel);
        };

        UILIB_API extern std::vector<std::function<bool(const std::string&, maxiSample&)>> loadPlugin;

        UILIB_API extern std::vector <OutEffect*> outEffects;

        UILIB_API extern std::vector <OutEffectEx*> outEffectExs;

        UILIB_API extern Listener thisListener;

        UILIB_API extern double(*speedOfSoundSampler)(const Math::Vec3& pos);

        UILIB_API extern unsigned char SampleNum;

        UILIB_API void init();

        UILIB_API SourceHandler load(const std::string& fileName);

        UILIB_API bool load(const std::string& fileName, std::shared_ptr<maxiSample> & sample);

        UILIB_API bool bind(const std::shared_ptr<maxiSample> & sample, SourceHandler& handler);

        UILIB_API void unInit();

        UILIB_API void openStream();

        namespace Effects
        {
            /*
            Balance
            名称:声道平衡
            作用:调节声道音量,或者作为Stereo的替代品.
            */
            class UILIB_API Balance : public OutEffect
            {
            public:
                std::vector<double> factors;
                Balance() = default;
                ~Balance() = default;
                virtual double operator()(double input, size_t channel);
            };

            /*
            Damping
            名称:衰减
            作用:计算声音衰减,不可作为回声的替代品.
            */
            class UILIB_API Damping : public OutEffect
            {
            private:
                std::vector<double> values;
            public:
                std::vector<double> factors;
                Damping() = default;
                ~Damping() = default;
                virtual double operator()(double input, size_t channel);
            };

            /*
            EQ
            名称:EQ均衡器
            作用:用于播放BGM或空间感渲染.参数以16bit为准.
            */
            class UILIB_API EQ : public OutEffect, public Effect
            {
            private:
                struct Point
                {
                    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
                    double value = 0.0;
                    bool end = false;
                };
                std::vector<Point> samplePoints;
                std::vector<double> freqs;
            public:
                double add = 0.0;
                struct Factors
                {
                    double facs[10];
                }
                EQFactor;
                //Preset from http://blog.sina.com.cn/s/blog_4cb5837d01018rax.html
                //                     31   62    125  250  500  1K   2K   4K   8K   16K
                const Factors bass = { { 6.0, 4.0, -5.0, 2.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0 } };
                const Factors rock = { { 6.0, 4.0, 0.0, -2.0, -6.0, 1.0, 4.0, 6.0, 7.0, 9.0 } };
                const Factors vocal = { { 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 4.0, 3.0, 3.0 } };
                //Interval
                const double interval[11][2] =
                {
                    {DBL_MIN, 31.0},
                    {31.0, 62.0},
                    {125.0, 250.0},
                    {250.0, 500.0},
                    {500.0, 1000.0},
                    {1000.0, 2000.0},
                    {2000.0, 4000.0},
                    {4000.0, 8000.0},
                    {8000.0, 16000.0},
                    {16000.0, DBL_MAX}
                };

                EQ() = default;
                ~EQ() = default;
                virtual double operator()(double input, size_t channel);
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Amplifier
            名称:差异放大器
            作用:放大各声道的差异,提高立体感,作为Stereo的补偿.
            */
            class UILIB_API Amplifier : public OutEffectEx
            {
            public:
                double factor = 1.0;
                Amplifier() = default;
                ~Amplifier() = default;
                virtual void operator()(double* input, size_t channel);
            };

            /*
            Stereo
            名称:3D效果器
            作用:通过声音衰减模型计算音量,是立体感的基础,与Listener的pos2搭配使用.
            */
            class UILIB_API Stereo : public EffectEx
            {
            public:
                Stereo() = default;
                ~Stereo() = default;
                double rolloffFactor = 1.0;
                double referenceDistance = 1.0;
                double maxDistance = 100.0;
                enum Mode
                {
                    INVERSE_DISTANCE,
                    INVERSE_DISTANCE_CLAMPED,
                    LINEAR_DISTANCE,
                    LINEAR_DISTANCE_CLAMPED,
                    EXPONENT_DISTANCE,
                    EXPONENT_DISTANCE_CLAMPED
                } dampMode = EXPONENT_DISTANCE_CLAMPED;
                virtual double operator()(double input, size_t channel, Source* thisSource);
            };

            /*
            Doppler
            名称:多普勒效应
            作用:通过多普勒效应模型计算频率,提高立体感.(要获得更好的体验请更换speedOfSoundSampler函数,并提高采样次数)
            */
            class UILIB_API Doppler : public Effect
            {
            public:
                Doppler() = default;
                ~Doppler() = default;
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Noise
            名称:噪音生成器
            作用:用于生成噪音并叠加在原波形上,用于播放自然声音.
            */
            class UILIB_API Noise : public Effect
            {
            public:
                Noise();
                ~Noise() = default;
                enum NoiseMode
                {
                    Add,
                    Factor
                } noiseMode = Factor;
                double factor = 0.1;
                virtual double operator()(double input, Source* thisSource);
            };

            /*
            Compressor
            Maxmilian自带效果器.
            */
            class UILIB_API Compressor : public Effect
            {
            public:
                Compressor() = default;
                ~Compressor() = default;
                maxiDyn compressor;
                virtual double operator()(double input, Source* thisSource);
            };
        }
    }
//#undef UILIB_API
//#define UILIB_API __declspec(dllexport)
}
