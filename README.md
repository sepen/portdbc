# portdbc

CRUX's portdb command line client

portdbc is a command line tool which provides an easy way to query the CRUX's
portdb (http://crux.nu/portdb) without the need of use a web browser for that.
  
It is written in C and uses libcurl(3) for obtaining the desired information
and after that parses them by using libxml(3) to show you the output result.
  
One of the advantages of this tool is that it can be combinated with shell
functions, I/O operations, command filters, etc. to exactly find what you are
looking for.


## Requirements

* libxml2 (http://xmlsoft.org)
* libcurl (http://curl.haxx.se)


## Installation

Build and install
```
$ autoconf
$ ./configure
$ make
$ sudo make install
```

## Usage
```
  portdbc <command> [arg]
  Where commands are:
    repos             Overview of all available repositories
    dups              List of duplicated ports
    search  <expr>    Show port names containing 'expr'
    list    <repo>    Show ports in repository
    getup   <repo>    Print httpup/rsync file for a repository
    help              Show this help information
    version           Show version information

  See the page manual for more information: portdbc(8)
```

## Examples

If you want to obtain the sync file from the kde4 repository:
```
$ portdbc getup kde4 > /etc/ports/kde4.rsync
```

To find xfce ports out of the xfce repository:
```
$ portdbc search xfce | grep -v ' xfce '
```

To list top 10 port duplicates
```
$ portdbc dupds | head -10
```