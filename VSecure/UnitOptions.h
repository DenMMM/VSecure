//---------------------------------------------------------------------------
#ifndef UnitOptionsH
#define UnitOptionsH
//---------------------------------------------------------------------------
class MOptions
{
private:
    bool LoadValue(HKEY hKey_, LPTSTR lpValueName_,
        DWORD dwType_, CONST BYTE *lpData_, DWORD cbData_);
public:
    DWORD DriverIndex;
    DWORD FrameRate;
    DWORD CodecHandler;
    LONG KeyRate;
    LONG DataRate;
    LONG Quality;
    int SpecSize;
    void *SpecData;
    char Directory[MAX_PATH];
    int NewFileTime;
    int FilesSize;

    bool Load();

    MOptions();
    ~MOptions();
};
//---------------------------------------------------------------------------
#endif
