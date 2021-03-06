#!/bin/bash
user=whoami
fftype=fftype

line=" --------------------------------------------------------------------------------------"
echo $line
echo "Starting $fftype analysis"
echo "Group CMS-HTT"
echo "Hephy Vienna"
echo $line

if [ -d "/data/$user/$fftype/preselection" ]; then
    if [ ! -d "/data/$user/$fftype/preselection/mt" ]; then
        mkdir -p /data/$user/$fftype/preselection/mt
    fi
    if [ ! -d "/data/$user/$fftype/preselection/et" ]; then
        mkdir -p /data/$user/$fftype/preselection/et
    fi
    if [ ! -d "/data/$user/$fftype/preselection/tt" ]; then
        mkdir -p /data/$user/$fftype/preselection/tt
    fi
else
    mkdir -p /data/$user/$fftype/preselection/mt
    mkdir -p /data/$user/$fftype/preselection/et
    mkdir -p /data/$user/$fftype/preselection/tt
fi

if [ -d "sim" ]; then
    if [ ! -d "sim/mt" ]; then
        mkdir -p sim/mt
    fi
    if [ ! -d "sim/et" ]; then
        mkdir -p sim/et
    fi
    if [ ! -d "sim/tt" ]; then
        mkdir -p sim/tt
    fi
else
    mkdir -p sim/mt
    mkdir -p sim/et
    mkdir -p sim/tt
fi

if [ -d "Images" ]; then
    if [ ! -d "Images/data_mt" ]; then
        mkdir -p Images/data_mt
    fi
    if [ ! -d "Images/data_et" ]; then
        mkdir -p Images/data_et
    fi
    if [ ! -d "Images/data_tt" ]; then
        mkdir -p Images/data_tt
    fi
    if [ ! -d "Images/mc_mt" ]; then
        mkdir -p Images/mc_mt
    fi
    if [ ! -d "Images/mc_et" ]; then
        mkdir -p Images/mc_et
    fi
    if [ ! -d "Images/mc_tt" ]; then
        mkdir -p Images/mc_tt
    fi
    if [ ! -d "Images/mc_woQCD_mt" ]; then
        mkdir -p Images/mc_woQCD_mt
    fi
    if [ ! -d "Images/mc_woQCD_et" ]; then
        mkdir -p Images/mc_woQCd_et
    fi
    if [ ! -d "Images/mc_woQCD_tt" ]; then
        mkdir -p Images/mc_woQCd_tt
    fi
    
    
else
    mkdir -p Images/data_mt
    mkdir -p Images/data_et
    mkdir -p Images/data_tt
    mkdir -p Images/mc_mt
    mkdir -p Images/mc_et
    mkdir -p Images/mc_tt
    mkdir -p Images/mc_woQCD_mt
    mkdir -p Images/mc_woQCD_et
    mkdir -p Images/mc_woQCD_tt
fi

if [ -d "fakefactor" ]; then
    if [ ! -d "fakefactor/data_mt" ]; then
        mkdir -p fakefactor/data_mt
    fi
    if [ ! -d "fakefactor/data_et" ]; then
        mkdir -p fakefactor/data_et
    fi
    if [ ! -d "fakefactor/data_tt" ]; then
        mkdir -p fakefactor/data_tt
    fi
    if [ ! -d "fakefactor/mc_mt" ]; then
        mkdir -p fakefactor/mc_mt
    fi
    if [ ! -d "fakefactor/mc_et" ]; then
        mkdir -p fakefactor/mc_et
    fi
    if [ ! -d "fakefactor/mc_tt" ]; then
        mkdir -p fakefactor/mc_tt
    fi
    if [ ! -d "fakefactor/mc_woQCD_mt" ]; then
        mkdir -p fakefactor/mc_woQCD_mt
    fi
    if [ ! -d "fakefactor/mc_woQCD_et" ]; then
        mkdir -p fakefactor/mc_woQCd_et
    fi
    if [ ! -d "fakefactor/mc_woQCD_tt" ]; then
        mkdir -p fakefactor/mc_woQCd_tt
    fi
    
else
    mkdir -p fakefactor/data_mt
    mkdir -p fakefactor/data_et
    mkdir -p fakefactor/data_tt
    mkdir -p fakefactor/mc_mt
    mkdir -p fakefactor/mc_et
    mkdir -p fakefactor/mc_tt
    mkdir -p fakefactor/mc_woQCD_mt
    mkdir -p fakefactor/mc_woQCD_et
    mkdir -p fakefactor/mc_woQCD_tt
fi

if [ ! -d "ff_2d" ]; then
    if [ ! -d "ff_2d/mt" ]; then
        if [ ! -d "ff_2d/mt/incl" ]; then
            mkdir -p ff_2d/mt/incl;
        fi
        if [ ! -d "ff_2d/mt/_btag" ]; then
            mkdir -p ff_2d/mt/_btag;
        fi
        if [ ! -d "ff_2d/mt/_nobtag" ]; then
            mkdir -p ff_2d/mt/_nobtag;
        fi
        if [ ! -d "ff_2d/mt/_btag_loosemt" ]; then
            mkdir -p ff_2d/mt/_btag_loosemt;
        fi
        if [ ! -d "ff_2d/mt/_btag_tight" ]; then
            mkdir -p ff_2d/mt/_btag_tight;
        fi
        if [ ! -d "ff_2d/mt/_nobtag_loosemt" ]; then
            mkdir -p ff_2d/mt/_nobtag_loosemt;
        fi
        if [ ! -d "ff_2d/mt/_nobtag_tight" ]; then
            mkdir -p ff_2d/mt/_nobtag_tight;
        fi
        
    else
        mkdir -p ff_2d/mt/incl;
        mkdir -p ff_2d/mt/_btag;
        mkdir -p ff_2d/mt/_nobtag;
        mkdir -p ff_2d/mt/_btag_loosemt;
        mkdir -p ff_2d/mt/_btag_tight;
        mkdir -p ff_2d/mt/_nobtag_loosemt;
        mkdir -p ff_2d/mt/_nobtag_tight;
    fi
    if [ ! -d "ff_2d/mt" ]; then
        if [ ! -d "ff_2d/et/incl" ]; then
            mkdir -p ff_2d/et/incl;
        fi
        if [ ! -d "ff_2d/et/_btag" ]; then
            mkdir -p ff_2d/et/_btag;
        fi
        if [ ! -d "ff_2d/et/_nobtag" ]; then
            mkdir -p ff_2d/et/_nobtag;
        fi
        if [ ! -d "ff_2d/et/_btag_loosemt" ]; then
            mkdir -p ff_2d/et/_btag_loosemt;
        fi
        if [ ! -d "ff_2d/et/_btag_tight" ]; then
            mkdir -p ff_2d/et/_btag_tight;
        fi
        if [ ! -d "ff_2d/et/_nobtag_loosemt" ]; then
            mkdir -p ff_2d/et/_nobtag_loosemt;
        fi
        if [ ! -d "ff_2d/et/_nobtag_tight" ]; then
            mkdir -p ff_2d/et/_nobtag_tight;
        fi
        
    else
        mkdir -p ff_2d/et/incl;
        mkdir -p ff_2d/et/_btag;
        mkdir -p ff_2d/et/_nobtag;
        mkdir -p ff_2d/et/_btag_loosemt;
        mkdir -p ff_2d/et/_btag_tight;
        mkdir -p ff_2d/et/_nobtag_loosemt;
        mkdir -p ff_2d/et/_nobtag_tight;
    fi
    if [ ! -d "ff_2d/tt" ]; then
        if [ ! -d "ff_2d/tt/incl" ]; then
            mkdir -p ff_2d/tt/incl;
        fi
        if [ ! -d "ff_2d/tt/_btag" ]; then
            mkdir -p ff_2d/tt/_btag;
        fi
        if [ ! -d "ff_2d/tt/_nobtag" ]; then
            mkdir -p ff_2d/tt/_nobtag;
        fi
        
    else
        mkdir -p ff_2d/tt/incl;
        mkdir -p ff_2d/tt/_btag;
        mkdir -p ff_2d/tt/_nobtag;
    fi
    
    
else
    mkdir -p ff_2d/mt/incl;
    mkdir -p ff_2d/mt/_btag;
    mkdir -p ff_2d/mt/_nobtag;
    mkdir -p ff_2d/mt/_btag_loosemt;
    mkdir -p ff_2d/mt/_btag_tight;
    mkdir -p ff_2d/mt/_nobtag_loosemt;
    mkdir -p ff_2d/mt/_nobtag_tight;
    mkdir -p ff_2d/et/incl;
    mkdir -p ff_2d/et/_btag;
    mkdir -p ff_2d/et/_nobtag;
    mkdir -p ff_2d/et/_btag_loosemt;
    mkdir -p ff_2d/et/_btag_tight;
    mkdir -p ff_2d/et/_nobtag_loosemt;
    mkdir -p ff_2d/et/_nobtag_tight;
    mkdir -p ff_2d/tt/incl;
    mkdir -p ff_2d/tt/_btag;
    mkdir -p ff_2d/tt/_nobtag;
fi




echo $line
