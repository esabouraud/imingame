    
		IMinGame

	Annoncez � vos contacts MSN � quels jeux vous jouez

	http://imingame.sourceforge.net/
	
IMinGame est bas� sur NowPlayingGames v1.13 de Pier Castonguay
(http://nowplayinggames.sytes.net/). Il est distribu� sous
license BSD.

* Compatibilit� :
	Windows 2K/XP/Vista 32 bits
	Jeux DirectX/OpenGL
	MSN / Live Messenger / Pidgin (avec Musictracker)

* Pr�requis :
	Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)
	or
	Microsoft Visual C++ Express 2008 SP1 pour compiler

* D�pendances :
	libcurl - the multiprotocol file transfer library  (http://curl.haxx.se/)
	the Expat XML Parser (http://expat.sourceforge.net/)
	
* Utiliser IMinGame

Une fois d�marr�, IMinGame met � jour votre statut MSN "Je suis en train
d'�couter" avec le nom du jeu auquel vous jouez.

Il devrait fonctionner avec d'autres logiciels de messagerie instantan�e
supportant l'interface d'envoi d'information musicale � MSN Messenger,
comme Pidgin (http://www.pidgin.im) avec le plugin musictracker
(http://code.google.com/p/pidgin-musictracker).

* Liste noire/blanche

Les applications peuvent �tre mises en liste noire ou blanche, identifi�es
par leur nom de processus. Une application d�tect�e peut �tre mise en liste
noire en cliquant sur le bouton "En liste noire". N'importe quelle application
peut �tre d�tect�e gr�ce � la liste blanche (pas seulement les jeux), pour
cela il faut glisser&d�poser le fichier exe associ� ou un raccourci dans
la fen�tre principale.

Les deux listes peuvent �galement �tre modifi�es manuellement � partir de l'�cran
d'options. Une fois les modifications effectu�es, sauver le fichier et ENSUITE
appuyer sur le bouton Appliquer.

Le format de la liste blanche est (sans <>):
<nom du processus>|<nom envoy� � MSN>
<nom du processus>|<nom envoy� � MSN>
...

Le format de la liste noire est (sans <>):
<nom du processus>
<nom du processus>
...

* Int�gration Steam

IMinGame peut r�cup�rer les donn�es publiques d'un utilisateur sur le site web
steamcommunity.com. Si cette fonctionnalit� est activ�e, lorsqu'un jeu steam
est lanc� le message de statut sera bas� sur le statut steam actuel. Un profil
steam public est requis, tel que
http://steamcommunity.com/id/robinwalker
ou
http://steamcommunity.com/profiles/76561197960435530
Copier la partie apr�s http://steamcommunity.com/ dans la fen�tre d'options et
cocher la case pour activer l'int�gration. Celle-ci utilise comme message :
<Nom du jeu> [Steam] sur <IP du serveur si applicable>.
L'IP du serveur est mise � jour toutes les 10 minutes.

