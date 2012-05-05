# norootforbuild


Name:           python-bioformats
Version:        0.0.1
Release:        0
License:        MIT
Vendor:         IMPB RAS
Packager:       Victor Yacovlev <victor@lpm.org.ru>
Source0:        bioformats.py
Group:		Developmet/Libraries/Python
BuildArch:	noarch
Requires:       %_bindir/python2.7
Requires:	python-biopython

Summary:        BioSymbol external data exchange support

%description
See 6.1.3.1 (9) of specification (appendix to state contract)

%prep


%build

%install
mkdir -p %buildroot/%python_sitelib
cp %SOURCE0 %buildroot/%python_sitelib

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%dir %python_sitelib
%python_sitelib/bioformats.py

