#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP2 - Problème du voyageur de commerce
#
#   AUTEUR :
#     HAOUAS, Mohammed Najib - 06 mars 2021
#
#   RÉSUMÉ DES CHANGEMENTS :
#     03/10/2021 - Correction problème génération des fichiers identiques
#     03/08/2021 - Disponibilité initiale.
#
#   USAGE :
#     Ce script génère les exemplaires requis pour le TP2 portant sur le problème du voyageur de commerce.
#
#     $ ./inst_gen.py [-h] -s NB_VILLES [-n NB_EXEMPLAIRES] [-x PRÉFIXE]
#
#     où :
#       * NB_BATIMENTS est la taille du problème et 
#       * NB_EXEMPLAIRES est le nombre d'exemplaires différents requis (par défaut 1).
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
                        action='store', required=True, metavar='NB_BATIMENTS', type=int)
    parser.add_argument("-n", "--nb-exemplaires", \
                        help="Représente le nombre d'exemplaires d'une même taille à générer", \
                        action='store', required=False, metavar='NB_EXEMPLAIRES', type=int)
    parser.add_argument("-x", "--prefixe", \
                        help="Ajouter le préfixe indiqué aux noms des fichiers", \
                        action='store', required=False, metavar='PREFIXE', type=str)

    args = parser.parse_args()
    if not args.nb_exemplaires:
        args.nb_exemplaires = 1
    if not args.prefixe:
        args.prefixe = ''
    else:
        args.prefixe = args.prefixe + '_'

    # Parameters
    max_coord = 2000

    for file_n in range(args.nb_exemplaires):
        # Record of generated points to avoid duplicates
        pdict = [(max_coord + 1) * [False] for _ in range(max_coord + 1)]

        # Preallocate
        res = [2 * [0] for _ in range(args.taille)]
        
        # Generate points
        for i in range(args.taille):
            res[i][0] = random.randint(0, max_coord)
            res[i][1] = random.randint(0, max_coord)

            # Point is duplicate? Regenerate.
            while pdict[res[i][0]][res[i][1]]:
                res[i][0] = random.randint(0, max_coord)
                res[i][1] = random.randint(0, max_coord)
            pdict[res[i][0]][res[i][1]] = True

        # Write
        with open(args.prefixe + 'N' + str(args.taille) + '_' + str(file_n),'w') as inst:
            inst.write("%d\n" % args.taille)

            for i in range(args.taille):
                inst.write("%d %d\n" % (res[i][0], res[i][1]))
