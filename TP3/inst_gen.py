#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP3 - Compétition, extraction d'or
#
#   AUTEUR :
#     HAOUAS, Mohammed Najib - 05 avril 2021
#
#   RÉSUMÉ DES CHANGEMENTS :
#     04/06/2021 - Disponibilité initiale.
#
#   USAGE :
#     Ce script génère les exemplaires requis pour le TP3.
#     Ces exemplaires ne sont qu'à titre indicatif et il est possible que
#     la génération des exemplaires d'évaluation se fasse selon des calculs
#     différents. Vous ne devez émettre aucune conjecture suite à la lecture
#     du code ci-dessous. Seuls les consignes incluses dans l'énoncé ou
#     communiquées par le chargé prévalent.
#
#     $ ./inst_gen.py [-h] -s NB_BLOCS_VERT NB_BLOCS_HORZ
#
#     où :
#       * NB_BLOCS_VERT est la profondeur de la coupe du terrain, N ;
#       * NB_BLOCS_HORZ est la largeur de la coupe du terrain, M.
#
#     Il est nécessaire de rendre ce script exécutable en utilisant chmod +x
#     Python 3.5 ou ultérieur recommandé pour lancer ce script.


import random
import argparse


if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--taille", \
                        help="Représente la taille du graphe à générer", \
                        nargs=2, action='store', required=True, metavar='NB_BATIMENTS', type=int)

    args = parser.parse_args()

    # Parameters
    max_value = 100
    max_cost = 50

    with open('N' + str(args.taille[0]) + '_' + 'M' + str(args.taille[1]),'w') as inst:
        inst.write("%d %d\n" % (args.taille[0], args.taille[1]))

        for i in range(args.taille[0]):
            for j in range(args.taille[1]):
                inst.write("%3.d " % random.randint(0, max_value))
            inst.write('\n')
        
        for i in range(args.taille[0]):
            for j in range(args.taille[1]):
                inst.write("%2.d " % random.randint(1, max_cost))
            inst.write('\n')
