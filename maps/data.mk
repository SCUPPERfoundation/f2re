# makefile with dependencies for fed2 map files 
# include in local makefile

$(DEST)/%.loc: ./%.loc; cp $< $@
$(DEST)/%.npc: ./%.npc; cp $< $@
$(DEST)/%.msg: ./%.msg; cp $< $@
$(DEST)/%.txt: ./%.txt; cp $< $@
$(DEST)/%.xev: ./%.xev; cp $< $@
$(DEST)/%.xob: ./%.xob; cp $< $@
