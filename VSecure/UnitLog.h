//---------------------------------------------------------------------------
#ifndef UnitLogH
#define UnitLogH
//---------------------------------------------------------------------------
class MLog;
//---------------------------------------------------------------------------
class MLog
{
private:
    char *File;
    int MaxSize;

public:
    bool SetFile(char *FileName_);
    bool SetSize(int Size_);
    bool WriteLine(char *Text_, bool Time_);

    MLog();
    ~MLog();
};
//---------------------------------------------------------------------------
#endif
