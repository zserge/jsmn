%define	origname jsmn

Name:		libjsmn
Version:	0.1
Release:	1%{?dist}
Summary:	minimalistic JSON parser in C

License:	MIT
URL:		https://github.com/zserge/jsmn
Source0:	%origname.tar.bz2
Source1:	libjsmn-CMakeLists.txt


%description
jsmn (pronounced like 'jasmine') is a minimalistic
JSON parser in C. It can be easily integrated into
resource-limited or embedded projects.


%prep
%setup -q -n %origname
install -pm 644 %SOURCE1 %_builddir/%origname/CMakeLists.txt

%build
%{cmake}


%install
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%doc README.md
%_libdir/*

%changelog

* Tue Sep 20 2016 Andrew Clark <andrewclarkii@gmail.com> 0.1-1
- initial build for CentOS
