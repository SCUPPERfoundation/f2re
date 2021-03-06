What is this?
-------------

You are sitting at your cluttered desk, writing the documentation for
a repository of C++ code, used to simulate a multiplayer space fantasy
game. You pause to take a sip of iced coffee, and reminisce about your
extremely online childhood, on Earth, late 20th Century.

This code originated in the early 21st Century; Alan Lenton put it into
production on [December 25th, 2003](https://www.ibgames.net/fed2/info/notes/10.html).
You take a brief break from your documentation, to prepare a screenshot
for later attachment to this repository. Lenton was able to solo dev an
MMO RPG given ~2 weeks, because he'd been involved with the inception of
the genre during the late 1970s and early 1980s. He refers to Federation
as the [longest continuously running multiplayer game](https://www.ibgames.net/alan/)
despite rebooting the codebase several times over those years.

This is what passes for strong critique in academic circles. What we
have is a partial historic record of the source code, to a 30+ year
long online tradition of collaborative world building. This annotated
documentation is a guide to the code, and some of the available external
context, as documented on the world wide web. The tradition of Federation
and similar games is a useful slice of pre-web internet history. The
author(s) here would like to give present and future scholars who didn't
live through the 300 Baud Modem Age, a useful summary of socio-cultural
factors reflected in this codebase.

A young, aspiring video game developer in 2021 probably doesn't want
to solo dev an MMO RPG from scratch. Lenton's ability & desire to do
so comes with caveats: he had [Fiona Craig](http://www.ibgames.net/fi/me.html)
and many other partners-in-crime over those 30 years.

Why is this?
------------
The Fed2 codebase is built around a community of people, bridging the
pre-web technologies from the BBS era, to the pre-HTML5 era, sometimes
referred to as the beginning of Web 2.0. The further away from 1970 you
were born, the less visceral this history will be. What matters?

* Backwards compatibility can mean greater accessibility
* Preserving and annotating code is under-taught
* Game engines we can fully understand are in short supply
* Old technology _had_ to be efficient / Constraint breeds creativity
* Content Creation Tools that we make ourselves?!

Because this multiplayer simulation was written to work both in the
terminal with "raw ASCII" and in the browser with a modified web client,
we can see more of the seams that come with **content creation** and
**content presentation**. Today we have design systems like Inform7 and
Twine which give the author a dramatically improved writing experience.
We also have tools like Unity3D and UnrealEngine that generate nearly
universal "web" experiences. These newer tools all still have some fiddly
bits that require integration, which is now a massive multi-billion
dollar industry all on its own. (Why is software so hard?)

Some questions that still need answering?
* What is Fed2 simulating?
* What is and isn't efficient?
* What difficulties remain to run a standalone classroom sim?
* How can we build on historical documentation / practices?
* Which modern web techniques are best applied to Fed2 client code?

How is this?
------------
"We Build Worlds" -- Interactive Broadcasting

"World building is about the _audience_ building a world" -- @joningold
https://twitter.com/joningold/status/1383376532677169156

The "core lore" of Federation, in all of its incarnations, is that
Magrathea exists and is a real place where you can build your own
custom planet. There is also some kind of hierarchical power in each
iteration that enforces some meta-rules about humans interacting with
humans on the Internet. The game engine simulates an economy with an in
game currency, as well as a "premium currency" that historically was
available for purchase externally, to support the game's server costs.

Prior to that, the game itself was financed via a business relationship
with America Online. Prior to that, it was supported by CompuNet, but
neither of these commercial editions of the simulator are publicly
available source code.

It is roughly possible to rewrite the Fed2 story universe to operate
from scratch with entirely different lore. Some aspects of the economy
are inescapable from the lore, but could be re-factored into a fantasy
scenario, given enough motivation.

Fundamentally, players are taught to earn Imperial Groats (ig), and
incentivized to explore the economy and initial story world. Because
this iteration of the game was designed from 2003 to 2018, some of the
design can be reconstructed by the people who were playing it at the
time. The first player owned planet (2004? 2005?) is a matter of real
historical record, recoverable from the web.

The *database record* of this event, is also a matter of real historical
record, but it is proprietary to Interactive Broadcasting. The community
reboot, aka F2CE, maintains a new database as of 2018 and beyond. A
classroom edition of Federation 2 allows for creation of isolated
databases. Since the *population* of this database happened over the
course of 15+ years, the design of each game rank was tuned to a real
historical player base.

A classroom edition of this game needs to be disconnected from the
economic engine, or the economic/hierarchy engine needs to be retuned
significantly, to be of particular interest. Access to purchase a planet
from Magrathea was gated essentially by "time in game" which... makes
sense when you are building a multi-national online community. Some of
the tuning needed can be "easily" accomplished with direct manipulation
of the SQLite database.

A big next/first step is studying the snapshot of changes made between
2018-2021 for the community edition. Big branch details TBD.
