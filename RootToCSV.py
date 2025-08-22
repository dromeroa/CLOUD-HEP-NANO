import uproot
import awkward as ak

# Archivo NanoAOD original
input_file = "BulgGrav/0969B537-2A5F-8F45-AD1B-3088383B278C.root"

# Abrir archivo y obtener el árbol 'Events'
with uproot.open(input_file) as f:
    tree = f["Events"]

    # Seleccionar las ramas que quieres copiar
    selected_branches = ["nFatJet", "FatJet_eta", "FatJet_mass", "FatJet_msoftdrop","FatJet_phi", "FatJet_pt","FatJet_tau1","FatJet_tau2","FatJet_tau3","FatJet_jetId", "FatJet_nConstituents"]  # agrega las que necesites

    # Leer esas ramas (devuelve un diccionario de arrays awkward)
    arrays = tree.arrays(selected_branches)

    arrays_lists = {k: arrays[k].to_list() for k in selected_branches}

# Guardar en nuevo archivo ROOT con solo las ramas seleccionadas
with uproot.recreate("filtered_signal.root") as fout:
    fout["Events"] = arrays_lists


