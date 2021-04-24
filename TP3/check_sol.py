#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP3 - Compétition, extraction d'or
#
#   AUTEUR :
#     HAOUAS, Mohammed Najib - 10 avril 2020
#
#   RÉSUMÉ DES CHANGEMENTS :
#      4/18/2021 - -d/--charger-sol devient obligatoire si stdin de ce script est connecté à un terminal (ie, aucun piping)
#      4/16/2021 - Ajout de l'option -d/--charger-sol pour permettre d'ouvrir un fichier de solution directement
#      4/13/2021 - Disponibilité initiale.
#
#   USAGE :
#     Ce script vérifie le stdout passé en pipe pour conformité avec les exigences du TP3, tel que rédigé à la session H21.
#     Ce même script sera utilisé par les correcteurs pour juger la qualité des programmes développés avec l'appel suivant :
#       $ (timeout 180s ./tp.sh -e FICHIER_EXEMPLAIRE; exit 0) | ./check_sol.py -e FICHIER_EXEMPLAIRE
#     où :
#       * "timeout 180s" interrompt, après 3 minutes (180 sec), l'exécution de...
#       * "./tp.sh -e FICHIER_EXEMPLAIRE" tel que...
#       * "FICHIER_EXEMPLAIRE" est l'adresse de l'exemplaire à résoudre.
#       * "; exit 0" remplace le code de sortie de timeout pour permettre à...
#       * "|" de pipe la sortie de "tp.sh" après interruption par timeout à...
#       * "./check_sol.py" qui prend des paramètres *obligatoirement identiques* à ceux passés à "tp.sh" mais aussi...
#       * admet, de façon facultative, plusieurs autres paramètres (voir 'help' en utilisant -h).
#
#   EXEMPLES D'USAGE :
#     $ (timeout 180s ./tp.sh -e N300_M500; exit 0) | ./check_sol.py -e N300_M500 -s sortie.out
#       cette commande exécute tp.sh pour un max de 180s et passe son affichage à ce présent script, appelé avec les mêmes paramètres pour vérifier le résultat.
#       L'affichage est par ailleurs sauvegardé dans un fichier texte "sortie.out" dans le même dossier.
#       Sauvegarder cet affichage de tp.sh permet de le revérifier ultérieurement, sans avoir à réexécuter le programme, en utilisant par exemple la commande ci-dessous.
#       Alternativement, on aurait pu sauvegarder seulement la dernière solution en utilisant -l/--derniere-sol
#
#     $ cat sortie.out | ./check_sol.py -e N300_M500
#       sortie.out contient l'affichage d'une exécution antérieure. L'utilisation de "cat" permet de vérifier la solution qu'il contient par ce présent script.
#       Prendre garde à utiliser les mêmes paramètres avec ce script que ceux qui ont été employés lors de la génération de "sortie.out".
#       Si sortie.out ne peut pas être contenu dans la pipe, utilisez -d/--charger-sol
#     
#     $ (timeout 180s ./tp.sh -e N300_M500; exit 0) > sortie.out
#     $ cat sortie.out | ./check_sol.py -e N300_M500
#       Alternativement, il est possible de lancer vos programmes dans un premier temps et d'enregistrer leur sortie avec ">"...
#       pour ensuite les vérifier plus tard avec la deuxième commande.
#
#   ATTENTION:
#     Pour que la commande :
#       $ (timeout 180s ./tp.sh -e FICHIER_EXEMPLAIRE; exit 0) | ./check_sol.py -e FICHIER_EXEMPLAIRE
#     prenne en compte toutes vos solutions, il est INDISPENSABLE de flush votre stdout (ie, l'affichage standard de votre programme) À CHAQUE FOIS qu'une solution est affichée.
#     Pour ce faire, par exemple, après chaque affichage d'une solution améliorante:
#       * pour python 3: appelez sys.stdout.flush() ou spécifiez l'argument flush=True dans print().
#       * pour C : stdout est flushed automatiquement après un saut de ligne (eg impression de '\n' ou appel à puts) ou appelez fflush(stdout).
#       * pour C++ : insérez (<<) à std::cout soit std::endl (qui flush automatiquement après un saut de ligne) soit std::flush.
#       * pour Java : System.out flush automatiquement à chaque saut de ligne ou appelez System.out.flush().
#
#     Il est nécessaire de rendre ce script exécutable en utilisant chmod +x
#     Python 3.5 ou ultérieur recommandé pour lancer ce script.


import sys
import re
import argparse


def load_instance(instance_path):
    with open(instance_path,'r') as instance_stream:
        # Process first line which defines problem characteristics
        line_one = instance_stream.readline()
        if not re.match(r"^[ \t]*\d+[ \t]*\d+[ \t]*$", line_one):
            return 1

        dim_i, dim_j = [int(x) for x in line_one.split()]

        gold_values = [None]*dim_i
        for line_num in range(dim_i):
            gold_values[line_num] = [int(x) for x in instance_stream.readline().split()]
            if len(gold_values[line_num]) != dim_j:
                return 1

        extraction_costs = [None]*dim_i
        for line_num in range(dim_i):
            extraction_costs[line_num] = [int(x) for x in instance_stream.readline().split()]
            if len(extraction_costs[line_num]) != dim_j:
                return 1

    return dim_i, dim_j, gold_values, extraction_costs


def validate_solution(raw_solution):
    target_pattern = r"\n*(?:(?:[ \t]*\d+[ \t]+\d+[ \t]*\n)*(?:[ \t]*\d+[ \t]+\d+[ \t]*)\n\n)*(?P<optim>(?:[ \t]*\d+[ \t]+\d+[ \t]*\n)*(?:[ \t]*\d+[ \t]+\d+[ \t]*))\n*"
    return re.fullmatch(target_pattern, raw_solution)


def parse_solution(raw_solution):
    solution_data = []

    for line in raw_solution.splitlines():
        line_contents = line.split()

        solution_data.append([int(line_contents[0]), int(line_contents[1])])

    return solution_data


# Error codes encapsulated into first element of tuple returned: 0 OK, 1 out of bounds, 2 reused, 3 precedence
def compute_objective(processed_solutions, dim_i, dim_j, processed_values, processed_costs):
    extracted = [[False]*dim_j for _ in range(dim_i)]
    objective = 0

    for block in processed_solutions:
        # Out of bounds
        if block[0] < 0 or block[0] >= dim_i or block[1] < 0 or block[1] >= dim_j:
            return (1, None, block, None)
        
        # Reused
        if extracted[block[0]][block[1]]:
            return (2, None, block, None)

        # Precedence, upper left
        if (block[0] - 1) >= 0 and (block[1] - 1) >= 0 and (block[1] - 1) < dim_j and not extracted[block[0] - 1][block[1] - 1]:
            return (3, None, block, [block[0] - 1, block[1] - 1])

        # Precedence, upper center
        if (block[0] - 1) >= 0 and block[1] >= 0 and block[1] < dim_j and not extracted[block[0] - 1][block[1]]:
            return (3, None, block, [block[0] - 1, block[1]])

        # Precedence, upper right
        if (block[0] - 1) >= 0 and (block[1] + 1) >= 0 and (block[1] + 1) < dim_j and not extracted[block[0] - 1][block[1] + 1]:
            return (3, None, block, [block[0] - 1, block[1] + 1])

        extracted[block[0]][block[1]] = True

        objective += processed_values[block[0]][block[1]] - processed_costs[block[0]][block[1]]

    return (0, objective, None, None)


if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-e", "--exemplaire", \
                        help="Représente l'exemplaire correspondant à la solution étudiée", \
                        action='store', required=True, metavar='FICHIER_EXEMPLAIRE')
    parser.add_argument("-d", "--charger-sol", \
                        help="Permet de lire une solution à partir d'un fichier à indiquer au lieu de stdin", \
                        action='store', required=sys.stdin.isatty(), metavar='FICHIER_SOLUTION')
    parser.add_argument("-s", "--enregistrer", \
                        help="Si indiqué, écrire stdin dans le fichier exigé", \
                        action='store', required=False, metavar='FICHIER_SORTIE')
    parser.add_argument("-l", "--derniere-sol", \
                        help="Si indiqué, écrire la dernière solution passée via stdin dans le fichier exigé", \
                        action='store', required=False, metavar='FICHIER_SORTIE')
    parser.add_argument("-z", "--disp-mrz", \
                        help="Si utilisé, affiche dans stdout une Machine-Readable Zone (MRZ) pour permettre l'automatisation de la collecte de données", \
                        action='store_true', required=False)
    parser.add_argument("-i", "--identification", \
                        help="Si indiqué, écrire l'id passé en argument dans la MRZ", \
                        action='store', required=False, metavar='ID')
    args = parser.parse_args()

    sol_raw_content = None
    if args.charger_sol:
        # Store solution file
        sol_raw_content = open(args.charger_sol, 'r').read()
    else:
        # Store pipe
        sol_raw_content = sys.stdin.read()

    # Save solution if requested
    try:
        if args.enregistrer:
            with open(args.enregistrer,'w') as f_output:
                f_output.write(sol_raw_content)
            print("Info : solution sauvegardée dans " + args.enregistrer + ".")
    except:
        print("Attention : impossible de sauvegarderla solution. Vérifiez les permissions d'écriture. Sauvegarde ignorée.", file=sys.stderr)

    # Load instance corresponding to solution
    instance_data = None
    try:
        instance_data = load_instance(args.exemplaire)
    except:
        print("Erreur : impossible d'ouvrir le fichier de l'exemplaire.", file=sys.stderr)

        if args.disp_mrz:
            print("\nCSV Machine Readable Format:")
            # ID, Instance, Size, nb blocks, objective
            print(args.identification, args.exemplaire, 'N/D', 'N/D', "Impossible d'ouvrir le fichier d'exemplaire", '#', sep=',', flush=True)
        sys.exit(1)
    
    if not instance_data or instance_data == 1:
        print("Erreur : problème de lecture du fichier d'exemplaire. "\
            "Vérifiez le chemin et/ou le contenu de l'exemplaire.", file=sys.stderr)

        if args.disp_mrz:
            print("\nCSV Machine Readable Format:")
            # ID, Instance, Size, nb blocks, objective
            print(args.identification, args.exemplaire, 'N/D', 'N/D', "Exemplaire avec format non valide", '#', sep=',', flush=True)
        sys.exit(1)

    # Check whether solution format is as expected, ie pairs of integers on each line, grouped between single empty lines as needed
    solution_match = validate_solution(sol_raw_content)
    if not solution_match:
        print("Erreur : solution vide ou les solutions fournies ont un format non valide. Revoyez la convention discutée dans l'énoncé.", file=sys.stderr)

        if args.disp_mrz:
            print("\nCSV Machine Readable Format:")
            # ID, Instance, Size, nb blocks, objective
            print(args.identification, args.exemplaire, instance_data[0]*instance_data[1], 'N/D', "Format de solution non valide ou vide", '#', sep=',', flush=True)
        sys.exit(1)

    # Extract last solution
    raw_last_solution = solution_match.group("optim")

    # Save last solution if requested
    try:
        if args.derniere_sol:
            with open(args.derniere_sol,'w') as f_output:
                f_output.write(raw_last_solution)
            print("Info : dernière solution sauvegardée dans " + args.derniere_sol + ".")
    except:
        print("Attention : impossible de sauvegarder la dernière solution. Vérifiez les permissions d'écriture. Sauvegarde ignorée.", file=sys.stderr)

    # Structure solution in memory
    resolution_data = parse_solution(raw_last_solution)
    
    # Check solution's consistency and compute objective
    res = compute_objective(resolution_data, instance_data[0], instance_data[1], instance_data[2], instance_data[3])
    if res[0] != 0:
        print("Erreur : la dernière solution fournie présente un problème de consistance.", file=sys.stderr)

        if res[0] == 1:
            print("Raison : le bloc " + str(res[2]) + " est représenté par des coordonnées hors-limites.", file=sys.stderr)
        elif res[0] == 2:
            print("Raison : le bloc " + str(res[2]) + " est réutilisé.", file=sys.stderr)
        elif res[0] == 3:
            print("Raison : le bloc " + str(res[2]) + " est sélectionné avant que le bloc " \
                  + str(res[3]) + " ne soit extrait avant lui.", file=sys.stderr)
        
        if args.disp_mrz:
            print("\nCSV Machine Readable Format:")
            # ID, Instance, Size, nb blocks, objective
            print(args.identification, args.exemplaire, instance_data[0]*instance_data[1], 'N/D', "Solution inconsistante " + str(res[0]), '#', sep=',', flush=True)
        sys.exit(1)

    # Satisfied by the solutions' presentation, display best objective
    print("OK : la valeur de l'objectif de la dernière (ie, meilleure) solution fournie est de " + str(res[1]) + ".\n")

    print("N*M = " + str(instance_data[0]) + "*" + str(instance_data[1]) + " = " + str(instance_data[0]*instance_data[1]) + " blocs.")
    print("dont " + str(len(resolution_data)) + " ont été sélectionnés (" + "{:.2f}".format(len(resolution_data)/instance_data[0]/instance_data[1]*100) + "%).")

    print("\nTaille de stdin en mémoire : " + str(sys.getsizeof(sol_raw_content)/1000/1000) + "M")
    print("Taille de la dernière solution structurée en mémoire : " + str(sys.getsizeof(resolution_data)/1000/1000) + "M")

    # Machine readable zone to facilitate automatic logging
    if args.disp_mrz:
        print("\nCSV Machine Readable Format:")
        # ID, Instance, Size, nb blocks, objective
        print(args.identification, args.exemplaire, instance_data[0]*instance_data[1], len(resolution_data), res[1], '#',sep=',', flush=True)
