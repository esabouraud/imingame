    
		IMinGame

	Annoncez à vos contacts MSN à quels jeux vous jouez

	http://imingame.sourceforge.net/
	
IMinGame est basé sur NowPlayingGames v1.13 de Pier Castonguay
(http://nowplayinggames.sytes.net/). Il est distribué sous
license BSD.

* Compatibilité :
	Windows 2K/XP/Vista 32 bits
	Jeux DirectX/OpenGL
	MSN / Live Messenger / Pidgin (avec Musictracker)

* Prérequis :
	Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)
	or
	Microsoft Visual C++ Express 2008 SP1 pour compiler

* Dépendances :
	libcurl - the multiprotocol file transfer library  (http://curl.haxx.se/)
	the Expat XML Parser (http://expat.sourceforge.net/)
	
* Utiliser IMinGame

Une fois démarré, IMinGame met à jour votre statut MSN "Je suis en train
d'écouter" avec le nom du jeu auquel vous jouez.

Il devrait fonctionner avec d'autres logiciels de messagerie instantanée
supportant l'interface d'envoi d'information musicale à MSN Messenger,
comme Pidgin (http://www.pidgin.im) avec le plugin musictracker
(http://code.google.com/p/pidgin-musictracker).

* Liste noire/blanche

Les applications peuvent être mises en liste noire ou blanche, identifiées
par leur nom de processus. Une application détectée peut être mise en liste
noire en cliquant sur le bouton "En liste noire". N'importe quelle application
peut être détectée grâce à la liste blanche (pas seulement les jeux), pour
cela il faut glisser&déposer le fichier exe associé ou un raccourci dans
la fenêtre principale.

Les deux listes peuvent également être modifiées manuellement à partir de l'écran
d'options. Une fois les modifications effectuées, sauver le fichier et ENSUITE
appuyer sur le bouton Appliquer.

Le format de la liste blanche est (sans <>):
<nom du processus>|<nom envoyé à MSN>
<nom du processus>|<nom envoyé à MSN>
...

Le format de la liste noire est (sans <>):
<nom du processus>
<nom du processus>
...

* Intégration Steam

IMinGame peut récupérer les données publiques d'un utilisateur sur le site web
steamcommunity.com. Si cette fonctionnalité est activée, lorsqu'un jeu steam
est lancé le message de statut sera basé sur le statut steam actuel. Un profil
steam public est requis, tel que
http://steamcommunity.com/id/robinwalker
ou
http://steamcommunity.com/profiles/76561197960435530
Copier la partie après http://steamcommunity.com/ dans la fenêtre d'options et
cocher la case pour activer l'intégration. Celle-ci utilise comme message :
<Nom du jeu> [Steam] sur <IP du serveur si applicable>.
L'IP du serveur est mise à jour toutes les 10 minutes.

