Name:       capi-media-audio-io
Summary:    An Audio Input & Audio Output library in Tizen Native API
%if 0%{?tizen_profile_mobile}
Version: 0.2.0
Release:    0
%else
Version: 0.2.9
Release:    0
%endif
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
%if "%{_repository}" == "wearable"
BuildRequires:  pkgconfig(mm-pcmsound)
%else
BuildRequires:  pkgconfig(mm-sound)
%endif
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(capi-base-common)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description
An Audio Input & Audio Output library in Tizen Native API

%package devel
Summary:  An Audio Input & Audio Output library in Tizen Native API (Development)
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel
An Audio Input & Audio Output library in Tizen Native API (DEV)

%prep
%setup -q


%build
%if 0%{?tizen_profile_mobile}
cd mobile
%else
cd wearable
%endif
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%if 0%{?tizen_profile_wearable}
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
%else
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
%endif

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%if 0%{?tizen_profile_mobile}
cd mobile
%else
cd wearable
%endif
mkdir -p %{buildroot}/usr/share/license
%if 0%{?tizen_profile_wearable}
cp LICENSE %{buildroot}/usr/share/license/%{name}
%else
cp LICENSE.APLv2 %{buildroot}/usr/share/license/%{name}
%endif
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-media-audio-io.so.*
%{_datadir}/license/%{name}
%if 0%{?tizen_profile_mobile}
%manifest mobile/capi-media-audio-io.manifest
%else
%manifest wearable/capi-media-audio-io.manifest
%endif

%files devel
%{_includedir}/media/audio_io.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-audio-io.so


