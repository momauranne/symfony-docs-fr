format
~~~~~~

**type**: ``integer`` ou ``string`` **default**: ``IntlDateFormatter::MEDIUM``

Option pass�e � la classe ``IntlDateFormatter``, utilis�e pour formatter
la saisie de l'utilisateur au bon format. C'est essentiel quand l'option
`widget`_  est d�finie comme ``single_text``, et elle d�finira la mani�re
dont l'utilisateur devra saisir ses donn�es. 
Par d�faut, le format est d�termin� selon la locale de l'utilisateur, mais
vous pouvez la surcharger en passant le format sous forme de chaine de
caract�res.

Pour plus d'informations sur les formats valides, lisez `Syntaxe des formats Date/Time`_.
Par exemple, pour afficher un simple champ texte qui attend une saisie au format
``yyyy-MM-dd``, utilisez l'option suivante::

    $builder->add('date_created', 'date', array(
        'widget' => 'single_text',
        'format' => 'yyyy-MM-dd',
    ));

.. _`Syntaxe des formats Date/Time`: http://userguide.icu-project.org/formatparse/datetime#TOC-Date-Time-Format-Syntax
