Salut ! Pour ta question sur le vecteur de décalage : dans notre cas, ce n'est pas 0. On utilise les vecteurs officiels de CEC2005 (mode « cec », le mode par défaut de notre main.cpp), les mêmes pour toutes les architectures. J'ai comparé nos valeurs une par une avec les données CEC2005 et il n'y a aucun écart.

Par contre, le code du prof, lui, met o = 0 en dur (x[i] - 0), donc l'optimum y est à l'origine. L'énoncé demande des fonctions « shifted CEC2005 », donc on a gardé les vrais vecteurs. Ça ne change rien à l'équité de la comparaison entre architectures, puisque le décalage est le même partout.

Si tu es dans le dilemme : le plus sûr est de garder CEC, et d'écrire dans le rapport que le code de départ utilisait o = 0 et qu'on a pris les vecteurs officiels. Tu peux aussi relancer un petit sous-ensemble avec --shift zero pour montrer que les conclusions ne changent pas.

Au fait, attention : dans le code du prof, le Griewank initialise le produit à 0, donc il est faux. Chez nous il part de 1.
