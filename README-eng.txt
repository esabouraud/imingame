    
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

* Dependencies :
	libcurl - the multiprotocol file transfer library  (http://curl.haxx.se/)
	the Expat XML Parser (http://expat.sourceforge.net/)
	
* Using IMinGame

Once run, IMinGame will start updating your MSN "Now Playing" (or "Show
what I'm listening to") status, with the name of the game you are playing.

It should work with other IM software implementing the 'Send "Now Playing"
information to MSN Messenger' interface, like Pidgin (http://www.pidgin.im)
with the musictracker plugin (http://code.google.com/p/pidgin-musictracker).

* Black/White list

Applications can be blacklisted or whitelisted based on their process name.
The application currently detected can be blacklisted by pressing the
"Blacklist it" button. Any application (not just games) can be detected 
if it is whitelisted, which is accomplished by drag&dropping its exe file
or shortcut into the main window.

Both can be edited from the settings dialog. Once it is done, save the file
THEN click the apply button.

The format for white list file is (without <>) :
<process name>|<name sent to MSN>
<process name>|<name sent to MSN>
...

The format for black list file is (without <>) :
<process name>
<process name>
...

* Steam integration

IMinGame can fetch public user data on the steamcommunity.com website. If this
feature is enabled, when a steam game is run the status message will be based
on the current steam status.