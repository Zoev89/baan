#ifndef IVIEW_H
#define IVIEW_H

class IView
{
public:
    enum MouseEventType
    {
        ButtonDrag,
        ButtonClick
    };
    enum MouseButtonType
    {
        LeftButton,
        RightButton
    };
    enum KeyBoardEventType
    {
        RightKey,
        LeftKey,
        UpKey,
        DownKey,
        DeleteKey
    };
    ~IView() = default;

    virtual void Draw() = 0;
    virtual bool HandleMouseEvents (MouseEventType type, MouseButtonType button,int x, int y) = 0;
    virtual bool HandleKeyBoardEvent(KeyBoardEventType key) = 0;

};

#endif // IVIEW_H
