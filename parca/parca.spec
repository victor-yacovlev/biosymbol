# norootforbuild


%define python_name_prefix python


Name:           parca
Version:        1.0.1
Release:        0
License:        MIT
Vendor:         IMPB RAS
Packager:       Victor Yacovlev <victor@lpm.org.ru>
Source:         %name-%version.tar.gz
Group:		Developmet/Libraries/Python

Requires:       %python_name_prefix-%name = %version
Requires:       %_bindir/python2.7

BuildRequires:  gcc-c++
BuildRequires:  cmake >= 2.8.3
BuildRequires:  pkgconfig(python-2.7)
BuildRequires:  boost-devel >= 1.44

Summary:        PAReto CAndidate sequence aligner

%description
PARCA is a global alignment tool for aminoacid sequences.
Instead of classical Smith-Waterman algorhitm (implemented
in EMBOSS and, partially, FASTA), PARCA results to a set
of alignments candidates. 

Usually you do not need PARCA to align high-gomologious
(at least 30\% initial identity) sequences, but PARCA
shows higher accuracy than Smith-Waterman algorhitm in
case of low-gomologious sequences.

%prep
%setup -q

%build
mkdir build
pushd build
cmake ../ -DCMAKE_INSTALL_PREFIX=%buildroot/%_prefix
make
popd

%install
pushd build
make install 
popd
mkdir -p %buildroot/%_bindir/
cp parca %buildroot/%_bindir/
echo '#!/bin/bash' >> %buildroot/%_bindir/bio_parca_soap_d
echo 'python -m parca.soap_server' >> %buildroot/%_bindir/bio_parca_soap_d
chmod a+x %buildroot/%_bindir/bio_parca_soap_d
mkdir -p %buildroot/etc/biosymbol/
mkdir -p %buildroot/etc/init.d/
#cp parca_soap.conf %buildroot/etc/biosymbol/
cp parca_server %buildroot/etc/init.d/
mkdir -p %buildroot/var/log/biosymbol/


%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%dir %_bindir
%_bindir/parca

%package -n %python_name_prefix-%name
Summary:        Python module for PARCA
Requires:       python >= 2.7

%description -n %python_name_prefix-%name
Python module for PARCA scripting

%files -n %python_name_prefix-%name
%defattr(-,root,root)
%dir %python_sitearch/parca
%python_sitearch/parca/__init__.py*
%python_sitearch/parca/MatrixInfo.py*
%python_sitearch/parca/__main__.py*
%python_sitearch/parca/_base.py*
%python_sitearch/parca/_custom_matrices.py*
%python_sitearch/parca/_util.py*
%python_sitearch/parca/_parca.so

%package -n %name-soap-server
Summary:	SOAP service for PARCA
Requires:	%python_name_prefix-%name = %version
Requires:	%python_name_prefix-mysql
Requires:	%python_name_prefix-pysqlite
Requires:	%python_name_prefix-soappy
Requires:	%python_name_prefix-SQLAlchemy

%description -n %name-soap-server
SOAP service usigng with MySQL database

%files -n %name-soap-server
%defattr(-,root,root)
%dir %python_sitearch/parca
%dir %_bindir
%dir /etc/biosymbol
%dir /etc/init.d
%python_sitearch/parca/soap_server.py*
%_bindir/bio_parca_soap_d
/etc/init.d/parca_server
%attr(-,nobody,nogroup) %dir /var/log/biosymbol

%post -n %name-soap-server
mkdir -p /etc/biosymbol/
if [ ! -f /etc/biosymbol/parca_soap.conf ]
then
    echo "HOST=localohst" > /etc/biosymbol/parca_soap.conf
    echo "PORT=8050" >> /etc/biosymbol/parca_soap.conf
    echo "DATABASE=sqlite:///:memory:" >> /etc/biosymbol/parca_soap.conf
    echo "USER=nobody" >> /etc/biosymbol/parca_soap.conf
    echo "GROUP=nogroup" >> /etc/biosymbol/parca_soap.conf
fi

