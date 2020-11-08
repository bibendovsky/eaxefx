# EAXEFX changelog


## [1.0.2] - 2020-12-14
### Added
- Support all EAX features (2.0/3.0/4.0/5.0) with few exceptions.
- Probe for extended filter gain extension (`AL_SOFTX_filter_gain_ex`).

### Changed
- Initialize EAX context on first call to `EAXGet` or `EAXSet`.
- Rewrite some standard classes with WinAPI to reduce the size of binaries.
- [Doom 3 Fix] Use console for messages.


## [1.0.1] - 2020-11-08
### Added
- Support `Soldier of Fortune II: Double Helix (2002)`.
- Support `Star Wars Jedi Knight II: Jedi Outcast (2002)`.
- Support `Star Wars Jedi Knight: Jedi Academy (2003)`.

### Changed
- Bundle the release with `OpenAL Soft v1.21.0` official build.

### Fixed
- Calculation of filter parameters.


## [1.0.0] - 2020-11-03
### Added
- First public release.
- Support `Doom 3 (2004)`.
- Support `Prey (2006)`.
- Support `Quake 4 (2005)`.
- `OpenAL Soft` Visual C++ 32-bit build `master/52d58a40234b8829801f0a587375eca91694c30f`.
