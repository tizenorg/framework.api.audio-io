#sbs-git:slp/api/audio-io capi-media-audio-io 0.1.0 da265a7364928d92084360809316e36f666f053f
Name:       capi-media-audio-io
Summary:    An Audio Input & Audio Output library in Tizen Native API
Version: 0.2.27
Release:    0
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(mm-sound)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(capi-base-common)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig
Requires(post): libprivilege-control

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
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/privilege-control

%post
/sbin/ldconfig
/usr/bin/api_feature_loader --verbose --dir=/usr/share/privilege-control

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-media-audio-io.so.*
%manifest capi-media-audio-io.manifest

%files devel
%{_includedir}/media/audio_io.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-audio-io.so


