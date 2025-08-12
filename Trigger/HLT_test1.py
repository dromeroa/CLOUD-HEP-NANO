import uproot
import awkward as ak

file = uproot.open("BulgGrav/0969B537-2A5F-8F45-AD1B-3088383B278C.root")
events = file["Events"]

# Cargar solo una rama de trigger y alguna física (por ejemplo, jets)
data = events.arrays(["HLT_IsoMu27", "Jet_pt", "Jet_eta", "Jet_phi"])

# Filtrar eventos que pasan el trigger
selected = data[data["HLT_IsoMu27"]]

print(f"Total de eventos: {len(data)}")
print(f"Eventos que pasaron HLT_IsoMu27: {len(selected)}")

