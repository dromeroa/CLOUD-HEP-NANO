import warnings
warnings.filterwarnings("ignore", category=FutureWarning)

import uproot
import pandas as pd

# Archivo NanoAOD original
input_file = "BulgGrav/0969B537-2A5F-8F45-AD1B-3088383B278C.root"

# Ramas seleccionadas para el filtrado
selected_branches = [
    "nFatJet", "FatJet_eta", "FatJet_mass", "FatJet_msoftdrop",
    "FatJet_phi", "FatJet_pt", "FatJet_tau1", "FatJet_tau2",
    "FatJet_tau3", "FatJet_jetId", "FatJet_nConstituents"
]

# Abrir el archivo ROOT original y obtener el árbol 'Events'
with uproot.open(input_file) as f:
    tree = f["Events"]

    # Leer las ramas seleccionadas en un diccionario de arrays awkward
    arrays = tree.arrays(selected_branches)

    # Convertir las arrays a listas y guardarlas en un nuevo archivo ROOT
    arrays_lists = {k: arrays[k].to_list() for k in selected_branches}

    with uproot.recreate("JetsFiltrado.root") as fout:
        fout["Events"] = arrays_lists

# Abrir el archivo ROOT filtrado
file = uproot.open("JetsFiltrado.root")

# Obtener el árbol 'Events'
tree = file["Events"]

# Convertir el árbol a un DataFrame de pandas
df = tree.arrays(library="pd")

# Guardar el DataFrame en un archivo CSV
df.to_csv("Jets_Signal_MC_propiedades.csv", index=False)

print("Archivo CSV guardado como 'Jets_Signal_MC_propiedades.csv'")


