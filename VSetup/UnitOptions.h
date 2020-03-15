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
    void *SpecData;
    int SpecSize;
    char Directory[MAX_PATH];
    int NewFileTime;
    int FilesSize;

    bool Save();
    bool Load();

    MOptions();
    ~MOptions();
};
//---------------------------------------------------------------------------
#endif
