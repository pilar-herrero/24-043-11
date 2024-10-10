#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <climits>
#include <cctype>

struct Despacho {
    int NroDock;
    char Producto[11];
    char Provincia[20];
    int Cantidad;

    Despacho() : NroDock(0), Cantidad(0) {
        memset(Producto, 0, sizeof(Producto));
        memset(Provincia, 0, sizeof(Provincia));
    }
};

bool esProductoValido(const std::string& producto) {
    if (producto.empty()) return false;
    for (char c : producto) {
        if (!std::isalpha(c) && !std::isspace(c)) {
            return false;
        }
    }
    return true;
}

void leerArchivo(const std::string& nombreArchivo, std::vector<Despacho>& despachos) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo) {
        std::cerr << "No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return;
    }

    Despacho despacho;
    while (archivo.read(reinterpret_cast<char*>(&despacho), sizeof(Despacho))) {
        despachos.push_back(despacho);
    }
    archivo.close();
}

void listarDespachosPorDock(const std::vector<Despacho>& despachos) {
    struct DockInfo {
        int cantidadDespachos = 0;
        std::vector<std::pair<std::string, int>> productos;
    };

    std::vector<DockInfo> docks(8);

    for (const auto& despacho : despachos) {
        if (despacho.NroDock >= 0 && despacho.NroDock < 8) {
            docks[despacho.NroDock].cantidadDespachos++;
            std::string producto(despacho.Producto);
            producto = producto.substr(0, producto.find('\0'));

            if (!esProductoValido(producto)) {
                continue;
            }

            bool encontrado = false;
            for (auto& prod : docks[despacho.NroDock].productos) {
                if (prod.first == producto) {
                    prod.second += despacho.Cantidad;
                    encontrado = true;
                    break;
                }
            }

            if (!encontrado) {
                docks[despacho.NroDock].productos.push_back({producto, despacho.Cantidad});
            }
        }
    }

    for (int i = 0; i < 8; ++i) {
        if (docks[i].cantidadDespachos > 0) {
            std::cout << "Dock " << i << ": " << docks[i].cantidadDespachos << " despachos" << std::endl;
            for (const auto& prod : docks[i].productos) {
                std::cout << "  Producto: " << prod.first << ", Cantidad Total: " << prod.second << std::endl;
            }
        }
    }
}

void dockConMenosDespachos(const std::vector<Despacho>& despachos) {
    int minDespachos = INT_MAX;
    int dockMenosDespachos = -1;
    std::vector<std::pair<std::string, int>> productosDock;

    for (int i = 0; i < 8; ++i) {
        int cantidadDespachos = 0;
        std::vector<std::pair<std::string, int>> productos;

        for (const auto& despacho : despachos) {
            if (despacho.NroDock == i) {
                cantidadDespachos++;
                std::string producto(despacho.Producto);
                producto = producto.substr(0, producto.find('\0'));

                if (!esProductoValido(producto)) {
                    continue;
                }

                bool encontrado = false;
                for (auto& prod : productos) {
                    if (prod.first == producto) {
                        prod.second += despacho.Cantidad;
                        encontrado = true;
                        break;
                    }
                }

                if (!encontrado) {
                    productos.push_back({producto, despacho.Cantidad});
                }
            }
        }

        if (cantidadDespachos > 0 && cantidadDespachos < minDespachos) {
            minDespachos = cantidadDespachos;
            dockMenosDespachos = i;
            productosDock = productos;
        }
    }

    if (dockMenosDespachos != -1) {
        std::cout << "Dock con menos despachos: " << dockMenosDespachos << " (" << minDespachos << " despachos)" << std::endl;

        auto productoMasDespachado = std::max_element(productosDock.begin(), productosDock.end(),
            [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                return a.second < b.second;
            });
        if (productoMasDespachado != productosDock.end()) {
            std::cout << "Producto más despachado: " << productoMasDespachado->first << " (" << productoMasDespachado->second << " unidades)" << std::endl;
        }
    }
}

int main() {
    std::vector<Despacho> despachos;

    leerArchivo("Datos.bin", despachos);

    listarDespachosPorDock(despachos);

    dockConMenosDespachos(despachos);

    return 0;
}