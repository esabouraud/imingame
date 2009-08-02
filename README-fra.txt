    
		IMinGame

	Annoncez à vos contacts MSN à quel jeux vous jouez

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

* Utiliser IMinGame

Une fois démarré, IMinGame met à jour votre statut MSN "Je suis en train
d'écouter" avec le nom du jeu auquel vous jouez.

Il devrait fonctionner avec d'autres logiciels de messagerie instantanée
supportant l'interface d'envoi d'information musicale à MSN Messenger,
comme Pidgin (http://www.pidgin.im) avec le plugin musictracker
(http://code.google.com/p/pidgin-musictracker).

* Listes noire/blanche

A partir de la version 0.2.0, sont implémentées des listes noire et blanche.
Les deux peuvent être modifiées à partir de l'écran d'options. Une fois
les modifications effectuées, sauver le fichier et appuyer sur le bouton
Appliquer. De plus, IMinGame peut être utilisé avec n'importe quelle
application grâce à la liste blanche, pas seulement les jeux.

Le format de la liste blanche est (sans <>):
<nom du processus>|<nom envoyé à MSN>
<nom du processus>|<nom envoyé à MSN>
...

Le format de la liste noire est (sans <>):
<nom du processus>
<nom du processus>
...

