#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP1 - Problème de la ligne d'horizon
#
#   AUTEUR :
#     HAOUAS, Mohammed Najib - 29 janvier 2021
#
#   RÉSUMÉ DES CHANGEMENTS :
#     01/30/2021 - Disponibilité initiale.
#
#   USAGE :
#     Ce script génère les exemplaires requis pour le TP1 portant sur le problème de la ligne d'horizon (Leetcode Hard).
#
#     $ ./inst_gen.py [-h] -s NB_BATIMENTS [-n NB_EXEMPLAIRES]
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
                        help="Représente le nombre de bâtiments à générer", \
                        action='store', required=True, metavar='NB_BATIMENTS', type=int)
    parser.add_argument("-n", "--nb-exemplaires", \
                        help="Représente le nombre d'exemplaires d'une même taille à générer", \
                        action='store', required=False, metavar='NB_EXEMPLAIRES', type=int)

    args = parser.parse_args()
    if not args.nb_exemplaires:
        args.nb_exemplaires = 1

    # Parameters
    max_width = 50
    max_dist = 30
    max_height = 300
    max_range = args.taille*max_dist

    # Generate
    for i in range(args.nb_exemplaires):
        with open('N' + str(args.taille) + '_' + str(i),'w') as inst:
            last_l = 0
            inst.write("%d\n" % args.taille)
            for _ in range(args.taille):
                l = random.randint(last_l, last_l + max_dist)
                r = random.randint(l+1, l + max_width)
                h = random.randint(1, max_height)

                inst.write("%d %d %d\n" % (l, r, h))

                last_l = l
