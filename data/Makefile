# Makefile for fed2 data files

DEST=/var/opt/fed2/data

all:$(DEST)/billing.dat $(DEST)/commodities.dat $(DEST)/equipment.dat \
$(DEST)/forbidden.dat $(DEST)/graphs.dat $(DEST)/maps.dat $(DEST)/parcels.dat \
$(DEST)/syndicates.xml $(DEST)/welcome.dat $(DEST)/routes.xml

$(DEST)/%.dat: ./%.dat; cp $< $@
$(DEST)/%.xml: ./%.xml; cp $< $@

