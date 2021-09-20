// Author: HenryAWE
// License: The 3-clause BSD License

namespace awe
{
    void InitSDL();

    void QuitSDL() noexcept;

    // WARNING: Special data is required by PhysFS on Android. Pay attention when porting to Android
    void InitPhysfs(const char* argv0 = nullptr);
    void QuitPhysfs() noexcept;

    void Prepare(const char* argv0);
}
