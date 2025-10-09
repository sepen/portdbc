# portdbc

CRUX's **portdb** command line client

portdbc is a command line tool which provides an easy way to query the CRUX's
[portdb](http://crux.nu/portdb) without the need of use a web browser for that.

It is written in C and uses **libcurl(3)** for data retrieval and **libxml(3)** for parsing XML responses.

One of its advantages is that it integrates smoothly with shell tools — you can combine it with pipes, filters, and scripts to automate port searches and maintenance tasks.


## Requirements

* [libxml2](http://xmlsoft.org)
* [libcurl](http://curl.haxx.se)


## Installation

Build and install from source:
```shell
autoconf
./configure
make
sudo make install
```

## Configuration

Starting with version 1.4, **portdbc** supports reading configuration from a file, allowing multiple PortDB URLs to be queried automatically.

By default, **portdbc** looks for a configuration file in: ```~/.portdbc.conf```. \
If not found, it falls back to a system default or the built-in URL.

Example `.portdbc.conf`:
```shell
# ~/.portdbc.conf

# Define one or more PortDB URLs (queried in order)
portdb_url = https://crux.nu/portdb/
portdb_url = https://crux-arm.nu/portdb/
```

Each **portdb_url** line adds another source to query. \
You can list multiple servers to mirror or extend your repositories.


## Usage

```shell
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

Retrieve the sync file from the _xfce4_ repository:
```shell
portdbc getup xfce4 | sudo tee /etc/ports/xfce4.rsync
```

Find ports containing _xfce_ but not in the _xfce4_ repo:
```shell
portdbc search xfce | grep -v ' xfce '
```

List the top 10 duplicated ports:
```shell
portdbc dupds | head -10
```