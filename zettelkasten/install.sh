#!/bin/sh
# must be run as root

# install.sh - will copy the ZMzettelkasten files to directories
# Bernd Holzhauer 2008
# last change: 2008-05-16

USER=`whoami`
if [ $USER != "root" ]; then
  echo "Script muss als user root ausgeführt werden"
  echo "ggf. mit sudo ./install.sh aufrufen ..."
  exit 1
fi

# create directories if not there
# --- pixmaps
if [ ! -d /usr/local/share/pixmaps ]; then
	mkdir /usr/local/share/pixmaps
fi
if [ ! -d /usr/local/share/pixmaps/ZMzettelkasten ]; then
	echo "create directories"
	mkdir /usr/local/share/pixmaps/ZMzettelkasten
fi
# --- locale
if [ ! -d /usr/local/share/locale ]; then
	mkdir /usr/local/share/locale
fi
if [ ! -d /usr/local/share/locale/en ]; then
	mkdir /usr/local/share/locale/en
fi
if [ ! -d /usr/local/share/locale/en/LC_MESSAGES ]; then
	mkdir /usr/local/share/locale/en/LC_MESSAGES
fi
# --- others
if [ ! -d /usr/local/share/applications ]; then
	mkdir /usr/local/share/applications
fi
if [ ! -d /usr/local/share/applications/ZMzettelkasten ]; then
	echo "create directories"
	mkdir /usr/local/share/applications/ZMzettelkasten
fi

# copy files to their places
echo "copy ZMzettelkasten files"
cp ZMzettelkasten /usr/local/bin/
# --- pixmaps
cp gnome-fs-bookmark.png /usr/local/share/pixmaps/ZMzettelkasten/
cp gnome-pdf.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_3d-light.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_delete-autofilter.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_filter-data-by-criteria.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_interaction.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_new-template.png /usr/local/share/pixmaps/ZMzettelkasten/
cp stock_todo.png /usr/local/share/pixmaps/ZMzettelkasten/
cp zmaus.png /usr/local/share/pixmaps/ZMzettelkasten/
cp zm_zettel.png /usr/local/share/pixmaps/ZMzettelkasten/
# --- locale
cp ZMzettelkasten.mo /usr/local/share/locale/en/LC_MESSAGES/
# --- others
cp hilfe /usr/local/share/applications/ZMzettelkasten/
cp zettelkasten.zk* /usr/local/share/applications/ZMzettelkasten/
cp export.hdr /usr/local/share/applications/ZMzettelkasten/
cp zettel.lang /usr/share/gtksourceview-1.0/language-specs/


echo "fertig"
exit 0
