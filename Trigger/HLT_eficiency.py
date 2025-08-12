import uproot
import numpy as np
import matplotlib.pyplot as plt

# Lista de archivos ROOT
files = ["BulgGrav/0969B537-2A5F-8F45-AD1B-3088383B278C.root", "BulgGrav/223DE7CC-81BF-0043-8562-70642DB08490.root", "BulgGrav/2AEBBA4B-BD1A-F04B-8B24-6E9769080426.root","BulgGrav/3B0DC90A-4FFD-FC4A-85D7-CBA90B53208B.root", "BulgGrav/3DA74196-4A44-9A45-BA3D-7416FECB5365.root", "BulgGrav/4B04795C-F451-6F43-80C0-1E0058D1F395.root", "BulgGrav/647D7905-1D81-624F-91D0-DBDBEA6C1EE8.root","BulgGrav/88716C27-F5BA-4F47-9BDB-969F9F555534.root","BulgGrav/8FA8AB72-B900-8944-A29B-225DF9705CC9.root","BulgGrav/B863EFF8-2066-F84C-B177-69F3AED929DE.root","BulgGrav/D8540788-62A6-5C46-805A-C129AB2F644D.root"]  # Reemplaza con tus nombres de archivo

# Definir el nombre del trigger path que deseas analizar (ajustalo según el nombre del path HLT que desees)
hlt_path = "HLT_AK8PFJet200"
# HLT_PFHT800
# HLT_FatJet200
# HLT_PFJet200_Mjj950
# HLT_PFJet200

# Inicializar listas vacías para acumular datos
pt_values_all = []
hlt_triggers_all = []

# Leer y procesar cada archivo ROOT
for file_name in files:
    # Abrir el archivo de NanoAOD
    file = uproot.open(file_name)
    
    # Obtener el árbol de eventos
    tree = file["Events"]
    
    # Extraer las variables de pT y el trigger (ajusta el nombre de las variables)
    pt_values = tree["FatJet_pt"].array()  # Asumiendo que tienes una variable llamada 'Jet_pt' para el pT
    hlt_triggers = tree[hlt_path].array()  # Variable del trigger en el path HLT
    
    # Agregar los datos del archivo a las listas acumuladoras
    pt_values_all.append(pt_values)
    hlt_triggers_all.append(hlt_triggers)
    
    # Cerrar el archivo
    file.close()

# Convertir las listas a arrays
pt_values_all = np.concatenate(pt_values_all)
hlt_triggers_all = np.concatenate(hlt_triggers_all)

# Definir rangos de pT para la gráfica
pt_bins = np.linspace(0, 1000, 150)  # Ajusta el rango de pT según el rango esperado en tu dataset
efficiency = []

# Calcular la eficiencia del trigger para cada rango de pT
for i in range(len(pt_bins) - 1):
    # Definir el rango de pT
    pt_min = pt_bins[i]
    pt_max = pt_bins[i + 1]

    # Filtrar los eventos con pT en el rango actual
    mask = (pt_values_all >= pt_min) & (pt_values_all < pt_max)

    # Calcular la eficiencia para el rango de pT
    total_events = np.sum(mask)
    triggered_events = np.sum(mask & hlt_triggers_all)

    # Calcular la eficiencia
    if total_events > 0:
        efficiency.append(triggered_events / total_events)
    else:
        efficiency.append(0.0)

# Graficar la eficiencia en función de pT
plt.figure(figsize=(10, 6))
plt.plot(pt_bins[:-1], efficiency, marker='x', linestyle='-', color='r', markersize=2)  # Cambiar linestyle a ''
plt.xlabel("pT [GeV]", fontsize=14)
plt.ylabel("Eficiencia del Trigger", fontsize=14)
plt.title(f"Eficiencia del Trigger {hlt_path} en función de pT", fontsize=16)
plt.grid(True)

# Guardar la gráfica como un archivo PDF
plt.savefig("eficiencia_trigger_multiple_files.pdf", format='pdf')

# Mostrar la gráfica
plt.show()

