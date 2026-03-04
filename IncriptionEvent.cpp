#include <iostream>
#include <string>
#include <unordered_map>
#include <forward_list>
#include <queue>
#include <cctype> // Pour la methode tolower
#include <Windows.h>
using namespace std;


// Interface IAccessible
class IAccessible {
protected:
    virtual bool verifierBadge() const = 0; 
};

// Classe abstraite Participant 
class Participant : public IAccessible {
protected:
    int id;
    string nom;

public:
    Participant(int id, string nom) : id(id), nom(nom) {} // Constructeur
    virtual void afficher() const = 0;
    
    // friend sur  modifierNomParticipant()
    friend void modifierNomParticipant(forward_list<Participant*>& liste, const string& ancienNom, const string& nouveauNom);

    bool operator<(const Participant& other) const {
        return this->id > other.id; // Pour priority_queue (ordre croissant d'ID)
    }

    virtual ~Participant() {} // Destructeur
};

// Classe Invite
class Invite : public Participant {
public:
    Invite(int id, string nom) : Participant(id, nom) {} // Destructeur

    // Affichage avec mention du rôle
    void afficher() const override {
        cout << "[Invite] " << nom << " (ID: " << id << ")\n";
    }

    bool verifierBadge() const override { return false; }
};

// Classe Employe
class Employe : public Participant {
public:
    Employe(int id, string nom) : Participant(id, nom) {}

    // Affichage avec mention du rôle
    void afficher() const override {
        cout << "[Employe] " << nom << " (ID: " << id << ")\n";
    }

    bool verifierBadge() const override { return true; }
};

// Classe VIP 
class VIP : public Participant {
public:
    VIP(int id, string nom) : Participant(id, nom) {}

    // Affichage avec mention du rôle
    void afficher() const override {
        cout << "[VIP] " << nom << " (ID: " << id << ")\n";
    }

    bool verifierBadge() const override { return true; }
};

// Fonction de comparaison insensible à la casse
bool equalLowercase(const string& nom1, const string& nom2) {
    // Verifier si leurs tailles sont différentes pour return false
    if (nom1.size() != nom2.size()) return false;
    // Boucle parcourant la chaine du nom1
    for (size_t i = 0; i < nom1.size(); ++i)
        // Verifier si la conversion de chaque caractere en minuscule de nom1 est différente de 
        // celle de nom2 pour return false 
        if (tolower(nom1[i]) != tolower(nom2[i])) return false;
    return true; // Si aucune des deux condition, return true
}

// Fonction amie pour modifier un nom dans la forward_list
void modifierNomParticipant(forward_list<Participant*>& liste, const string& ancienNom, const string& nouveauNom) {
    // Iterateur pointant vers une case memoire contenant Participant, pas directement vers un Participant
    for (forward_list<Participant*>:: iterator it = liste.begin(); it != liste.end(); it++) {
       
        if (equalLowercase((*it)->nom, ancienNom)) {
            (*it)->nom = nouveauNom;
            cout << "Nom modifie avec succes.\n";
            return; // S'arrêter dès qu'on retrouver le nom
        }
    }
    cout << "Aucun invite trouve avec ce nom.\n";
}

// Classe template GestionnaireEvenement
template <typename T>
class GestionnaireEvenement {
private:
    unordered_multimap<string, T*> mapEvenement;
    forward_list<T*> listeInscrits;
    priority_queue<T*> filePriorite;

public:
    void inscrire(string evenement, T* p) {
        mapEvenement.insert({evenement, p});
        listeInscrits.push_front(p);
        filePriorite.push(p);
        
    }

    void modifierNomParticipant(string& ancienNom, string& nouveauNom) {
        // Appel de la methode friend de la classe Participant
        ::modifierNomParticipant(listeInscrits, ancienNom, nouveauNom);
    }

    void afficherParPriorite() {
        
        priority_queue<T*> copie = filePriorite;
        while (!copie.empty()) {
            copie.top()->afficher(); // Grâce au polymorphisme appel de la bonne version de afficher(), selon INVITE, EMPLOYE et VIP
            copie.pop(); // Supprimer l'élément courant déjà affiché pour passer au suivant
        }
    }

    void nettoyage() {
        // Liberation de memoire avec une boucle parcourant la liste des participants
        for (auto it = listeInscrits.begin(); it != listeInscrits.end(); it++){
            T* p = *it;
            delete p; 
        }
        mapEvenement.clear(); // Pour liberer la liste des évènements
        listeInscrits.clear();
        // Tant que la liste par priorité n'est pas vide effacer l'élément au sommet
        while (!filePriorite.empty()) filePriorite.pop();
    }
};


int main() {
    SetConsoleOutputCP(CP_UTF8);
    GestionnaireEvenement<Participant> gestionnaire;

    // Création des participants
    Participant* p1 = new Invite(2, "Bob");
    Participant* p2 = new Employe(4, "Diana");
    Participant* p3 = new Employe(3, "Charlie");
    Participant* p4 = new Invite(1, "Alice");
    Participant* p5 = new VIP(5, "Eve");

    // Inscriptions de 5 participants à un événement
    gestionnaire.inscrire("Tech2025", p1);
    gestionnaire.inscrire("Tech2025", p2);
    gestionnaire.inscrire("Tech2025", p3);
    gestionnaire.inscrire("Tech2025", p4);
    gestionnaire.inscrire("Tech2025", p5);

    // Affichage initial des participants 
    cout << "Participants (ordre de priorité) : \n";
    gestionnaire.afficherParPriorite();

    // Modification du nom par saisie utilisateur
    string ancienNom, nouveauNom = "Jack";
    cout << "\nDonnez le nom de l'invite a modifier : ";
    cin >> ancienNom;
    gestionnaire.modifierNomParticipant(ancienNom, nouveauNom);

    // Réaffichage après modification
    cout << "\nParticipants (ordre de priorite) : \n";
    gestionnaire.afficherParPriorite();

    // Nettoyage
    gestionnaire.nettoyage();

    return 0;
}