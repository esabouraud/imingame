    
		IMinGame

	Tell your MSN friends what games you are playing.

	http://imingame.sourceforge.net/
	
IMinGame is based on Pier Castonguay's NowPlayingGames v1.13
(http://nowplayinggames.sytes.net/). It is released under a BSD licence.

* Compatibility :
	Windows 2K/XP/Vista 32 bits
	DirectX/OpenGL games
	MSN / Live Messenger / Pidgin (with Musictracker)

* Requirements :
	Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)
	or
	Microsoft Visual C++ Express 2008 SP1 to build from source

* Using IMinGame

Once run, IMinGame will start updating your MSN "Now Playing" (or "Show
what I'm listening to") status, with the name of the game you are playing.

It should work with other IM software implementing the 'Send "Now Playing"
information to MSN Messenger' interface, like Pidgin (http://www.pidgin.im)
with the musictracker plugin (http://code.google.com/p/pidgin-musictracker).

* Black/White lists

As of version 0.2.0 it implements process blacklisting and whitelisting.
Both can be edited from the settings dialog. Once it is done, save the file
then click the apply button. It should be noted that IMinGame can be used
with any application thanks to the whitelist, not just games.

The format for white list file is (without <>) :
<process name>|<name sent to MSN>
<process name>|<name sent to MSN>
...

The format for black list file is (without <>) :
<process name>
<process name>
...
