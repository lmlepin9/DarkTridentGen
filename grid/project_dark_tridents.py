#! /usr/bin/env python
######################################################################
#
# Name: project.py
#
# Purpose: Production project script.
#
# Created: 11-Sep-2012  Herbert Greenlee
#
# Usage:
#
# project.py <options>
#
# Project options:
#
# --xml <-|file|url>  - Xml file containing project description.
# --project <project> - Project name (required if xml file contains
#                       more than one project description).
# --stage <stage>[,<stage2>,...] - Project stage(s) (required if project
#                       contains more than one stage).
# --tmpdir <tempdir>  - Override TMPDIR internally.  If TMPDIR is set
#                       use ifdh cp instead of xrootd for accessing
#                       content of root files in dCache.
#
# XML stage configuration overrides.  Note that most of these options, except --inputdef,
# are passed directly to jobsub_submit.  Refer to jobsub_submit documentation for
# additional details.
#
# --lines <arg>        - Override stage element <lines>...</lines>.
# --site <site>        - Override stage element <site>...</site> (comma-separated list).
# --cpu <ncpus>        - Override stage element <cpu>...</cpu> (integer # of cpus).
# --disk <disk>        - Override stage element <disk>...</disk> (value and unit).
# --memory <mem>       - Override stage element <memory>...</memory> (MB).
# --inputdef <defname> - Override stage element <inputdef>...</inputdef>.  Any existing
#                        element of type <inputfile> or <inputlist> is nullified.
#
# Pubs options (combine with any action option).
#
# --pubs <run> <subrun> [<version>] - Modifies selected stage to specify pubs mode.
#                                     The <subrun> can be a range or comma-separated
#                                     list of subruns.
#
# Actions (specify one):
#
# [-h|--help]  - Print help (this message).
# [-xh|--xmlhelp] - Print xml help.
# --submit     - Submit all jobs for specified stage.
# --recur      - Input dataset is recursive.  Used in conjunction with --submit,
#                allows job submission even if output directories are not empty.
#                Also forces a new snapshot in case of input from sam.
# --check      - Check results for specified stage and print message.
# --checkana   - Check analysis results for specified stage and print message.
# --shorten    - Shorten root filenames to have fewer than than 200 characters.
# --fetchlog   - Fetch jobsub logfiles (jobsub_fetchlog).
# --mergehist  - merge histogram files using hadd -T
# --mergentuple- merge ntuple files using hadd
# --merge      - merge non-ART root files using the specified merging program in the XML file
#                (default hadd -T)
# --status     - Print status of each stage.
# --makeup     - Submit makeup jobs for specified stage.
# --clean      - Delete output from specified project and stage and following stages.
# --clean_one  - Delete output from specified project and stage (don't clean following stages).
# --declare    - Declare files to sam.
# --add_locations    - Check sam disk locations and add missing ones.
# --clean_locations  - Check sam disk locations and remove non-existent ones.
# --remove_locations - Remove all sam disk locations, whether or not file exists.
# --upload     - Upload files to enstore.
# --define     - Make sam dataset definition.
# --undefine   - Delete sam dataset definition.
# --audit      - compare input files to output files and look for extra
#                or misssing files and take subsequent action
#
# --declare_ana          - Declare analysis files to sam.
# --add_locations_ana    - Check sam analysis file disk locations and add missing ones.
# --clean_locations_ana  - Check analysis file sam disk locations and remove non-existent ones.
# --remove_locations_ana - Remove all analysis sam disk locations, whether or not file exists.
# --upload_ana           - Upload analysis files to enstore.
# --define_ana           - Make sam dataset definition for analysis files.
#
# Information only actions:
#
# --dump_project - Dump project object (dumps all stages).
# --dump_stage   - Dump stage object.
# --dryrun       - When combined with --submit or --makeup, do prep and show submit command,
#                  but don't submit jobs.
# --nocheck    - Parse xml with reduced checks.  This is mainly useful when
#                combined with one of the dump options.
#
# --outdir     - Print the name of the output directory for stage.
# --logdir     - Print the name of the log directory for stage.
# --workdir    - Print the name of the work directory for stage.
# --bookdir    - Print the name of the bookkeeping directory for stage.
# --fcl        - Print the fcl file name and version for stage.
# --defname    - Print sam dataset definition name for stage.
# --input_files        - Print all input files.
# --check_submit       - Run presubmission check script, if any.
# --check_input        - Do all standard input file checks.
#
# --check_declarations - Check whether data files are declared to sam.
# --test_declarations  - Print a summary of files returned by sam query.
# --check_locations    - Check sam locations and report the following:
#                        a) Files that lack any location.
#                        b) Disk locations that can be added.
#                        c) Incorrect disk locations that should be removed.
# --check_tape         - Check sam tape locations.
#                        Reports any files that lack tape (enstore) locations.
# --check_definition   - Reports whether the sam dataset definition associated
#                        with this project/stage exists, or needs to be created.
# --test_definition    - Print a summary of files returned by dataset definition.
#
# --check_declarations_ana - Check whether analysis files are declared to sam.
# --test_declarations_ana  - Print a summary of analysis files returned by sam query.
# --check_locations_ana    - Check sam locations for analysis files and report the
#                            following:
#                            a) Files that lack any location.
#                            b) Disk locations that can be added.
#                            c) Incorrect disk locations that should be removed.
# --check_tape_ana         - Check analysis file sam tape locations.
#                            Reports any files that lack tape (enstore) locations.
# --check_definition_ana   - Reports whether the sam analysis dataset definition
#                            associated with this project/stage exists, or needs to
#                            be created.
# --test_definition_ana    - Print a summary of files returned by analysis dataset
#                            definition.
#
######################################################################
#
# XML file structure
# ------------------
#
# The xml file must contain one or more elements with tag "project."
#
# The project element must have attribute "name."
#
# The following element tags withing the project element are recognized.
#
# <numevents> - Total number of events (required).
# <numjobs> - Number of worker jobs (default 1).  This value can be
#             overridden for individual stages by <stage><numjobs>.
# <maxfilesperjob> - Maximum number of files to deliver to a single job
#             Useful in case you want to limit output file size or keep
#             1 -> 1 correlation between input and output. can be overwritten
#             by <stage><maxfilesperjob>
# <ups>     - Override top level ups products (repeatable).
# <os>      - Specify batch OS (comma-separated list: SL5,SL6).
#             Default let jobsub decide.
#
#             If the singularity flag is false, this option is passed directly as 
#             jobsub_submit option --OS.
#
#             If singularity flag is true, this option is used to specify the
#             singularity image, passed via jobsub_submit --lines.
#
#             A singularity image can be specified as an absolute or relative path
#             of the image file, or as an alias, such as sl6, sl7, el8.
#             A singularity image alias can be upper or lower case.
#             The alias selects an image file in
#             directory /cvmfs/singularity.opensciencegrid.org/fermilab.
#
# <server>  - Jobsub server (expert option, jobsub_submit --jobsub-server=...).
#             If "" (blank), "-" (hyphen), or missing, omit --jobsub-server
#             option (use default server).
# <resource> - Jobsub resources (comma-separated list: DEDICATED,OPPORTUNISTIC,
#              OFFSITE,FERMICLOUD,PAID_CLOUD,FERMICLOUD8G).
#              Default: DEDICATED,OPPORTUNISTIC.
# <role>    - Role (normally Analysis or Production).  This element overrides the
#             default role-determining algorithm in larbatch_utilities.get_role().
# <lines>   - Arbitrary condor commands (expert option, jobsub_submit --lines=...).
# <site>    - Specify sites (comma-separated list, default jobsub decides).
# <blacklist> - Blacklist sites (comma-separated list, default jobsub decides).
#
# <cpu>     - Number of cpus (jobsub_submit --cpu=...).
# <disk>    - Amount of scratch disk space (jobsub_submit --disk=...).
#             Specify value and unit (e.g. 50GB).
# <memory>  - Specify amount of memory in MB (jobsub_submit --memory=...).
#
# <script>  - Name of batch worker script (default condor_lar.sh).
#             The batch script must be on the execution path.
# <startscript> - Name of batch worker start project script (default condor_start_project.sh)
#             Must be on execution path.
# <stopscript> - Name of batch worker stop project script (default condor_start_project.sh)
#             Must be on execution path.
#
#
# <larsoft> - Information about larsoft release.
# <larsoft><tag> - Frozen release tag (default "development").
# <larsoft><qual> - Build qualifier (default "debug", or "prof").
# <larsoft><local> - Local test release directory or tarball (default none).
# <version> - Specify project version (default same as <larsoft><tag>).
#
# <filetype> - Sam file type ("data" or "mc", default none).
# <runtype>  - Sam run type (normally "physics", default none).
# <runnumber> - Sam run number (default nont).
# <parameter name="parametername"> - Specify experiment-specific metadata parameters
#
# <merge>    - special histogram merging program (default "hadd -T",
#              can be overridden at each stage).
#              Set to "1" to generate merging metadata for artroot files.
# <anamerge> - Set to "1" to generate merging metadata for analysis files.
#              
# <check>    - Do on-node validation and sam declaration (0 or 1, default 0).
# <copy>     - Copy validated root files to FTS (0 or 1, default 0).
# <cvmfs>    - Cvmfs flag (0 or 1, default 1).  If nonzero, add option
#              "--append_condor_requirements='(TARGET.HAS_CVMFS_<experiment>_opensciencegrid_org==true)'"
# <stash>    - Stash cache flag (0 or 1, default 1).  If nonzero, add option
#              "--append_condor_requirements='(TARGET.HAS_CVMFS_<experiment>_osgstorage_org==true)'"
# <singularity> - Singularity flag (0 or 1, default 1).
#
# <stage name="stagename" base="basestage"> - Information about project stage.
#             There can be multiple instances of this tag with different name
#             attributes.  The name attribute is optional if there is
#             only one project stage.  The base attribute is also optional.
#             If present, it specifies a "base stage" which supplies default
#             values for all unspecified xml tags.
# <stage><batchname> - If present and not empty, override default batch job name.
# <stage><fcl> - Name of fcl file (required).
#                Search $FHICL_FILE_PATH, <fcldir>, or specify full path.
#                Repeatable.
#                See below for additional information about multiple fcls (substages).
# <stage><outdir> - Output directory (required).  A subdirectory with the
#             project name is created underneath this directory.  Individual
#             batch workers create an additional subdirectory under that with
#             names like <cluster>_<process>.  Output data (root) files
#             generated by batch jobs are stored in this directory.  This
#             directory should be grid-accessible.
# <stage><logdir> - Log directory (optional).  If not specified, default to
#             be the same as the output directory.  A directory structure
#             is created under the log directory similar to the one
#             under the output directory.  Non-data (non-root, usually small)
#             files generated by batch jobs are stored in this directory.
#             This directory should be grid-accessible.
# <stage><workdir> - Work directory (required).  This directory acts as the
#             submission directory for the batch job.  Fcl file, batch
#             script, and input file list are copied here.  A subdirectory with
#             the name of the project and "/work" are appended to this path.
#             This directory should be grid-accessible.
# <stage><bookdir> - Bookkeeping directory (optional).  If not specified, default
#             to be the same as the log directory.  A directory
#             structure is created under the bookkeeping directory similar
#             to the one under the output directory.  This directory is used
#             to store bookkeeping files generated by this script.  It does not
#             need to be grid-accessible.  Ideally the bookkeeping directory
#             should be on a local disk.
# <stage><dirsize> - Specify maximum directory size.  No effect unless <dirlevels>
#              is greater than zero.
# <stage><dirlevels> - Specify number of extra directory levels (default 0).
# <stage><inputfile> - Specify a single input file (full path).  The number
#             of batch jobs must be one.
# <stage><inputlist> - Specify input file list (a file containing a list
#             of input files, one per line, full path).
# <stage><inputmode> - Specify input file tyle. Default is none which means
#             art root file. Alternative is textfile
# <stage><inputdef>  - Specify input sam dataset definition.
#
#             It is optional to specify an input file or input list (Monte
#             Carlo generaiton doesn't need it, obviously).  It is also
#             optional for later production stages.  If no input is specified,
#             the list of files produced by the previous production stage
#             (if any) will be used as input to the current production stage
#             (must have been checked using option --check).
# <stage><inputstream> - Specify input stream.  This only effect of this
#             parameter is to change the default input file list name from
#             "files.list" to "files_<inputstream>.list."  This parameter has
#             no effect if any non-default input is specified.
# <stage><previousstage> - Specify the previous stage name to be something other
#             than the immediate predecessor stage specified in the xml file.
#             This parameter only affects the default input file list.  This
#             parameter has no effect if any non-default input is specified.
#             Specify as "none" (or any nonexistent stage) to prevent generation
#             of any default input (i.e. for noninitial generator stages).
# <stage><filelistdef> - Evaluate input sam definition using separated queries
#                        (may reduce load on sam database).
# <stage><mixinputdef> - Specify mix input from a sam dataset.
# <stage><pubsinput> - 0 (false) or 1 (true).  If true, modify input file list
#                      for specific (run, subrun, version) in pubs mode.  Default is true.
# <stage><maxfluxfilemb> - Specify GENIEHelper fcl parameter MaxFluxFileMB (default 500).
#                          Specifying this parameter as 0 will inhibit genie flux fcl
#                          overrides, which may be useful for non-genie generators.
#
#
# <stage><recur>      - Recursive flag (0 or 1).  Same as command line option --recur.
# <stage><recurdef>   - Specify recursive (aka draining) input dataset name.  Can be
#                       a predefined dataset definition or project.py can define it
#                       for you.
#
#                       The dataset specified by <recurdef> is used as input in preference
#                       to <inputdef> (if specified).
#
#                       This element also implicitly sets the recursive flag.
#
#                       If you want project.py to create a recursive dataset definition
#                       for you, specify both <recurdef> and <inputdef>.  Then
#                       project.py will create a dataset definition (if one doesn't exist)
#                       using <intputdef> as base, and adding optional "minus" and/or
#                       "with limit" clause in the sam dimension.
#
# <stage><recurtype>  - Specify the type of minus clause to use in a an automatically
#                       generated recursive dataset definition.  If this element is
#                       missing, the generated dataset definition will not include
#                       a minus clause.
#
#                       Allowed values are:
#
#                       none      - Don't generate a minus clause.
#                       snapshot  - "minus snapshot_for_project_name ...".
#                       consumed  - "minus (project_name ... and consumed_status consumed)"
#                       child     - "minus isparentof: ( ... )" using artroot data tier.
#                       anachild  - "minus isparentof: ( ... )" using analysis data tier.
#
# <stage><recurlimit> - Specify an integer value for "with limit" clause.  If this
#                       element is missing or the value is zero, the generated dataset
#                       definition will not include a "with limit" clause.
#
# <stage><singlerun>  - Single run flag.  If nonzero, limit input to come from a single
#                       run.  The run is based on a randomly selected file.
#
# <stage><prestart>   - Prestart flag.  If specified and nonzero, start the sam project
#                       in this script, instead of in a batch job.
#
# <stage><activebase> - If this element is present and nonempty, define or update an
#                       active projects dataset "<activebase>_active,"
#                       where <activebase> is the value of this element.
#                       Do this in the input checking phase (e.g. prior to job submission)
#                       in function stagedef.checkinput.
#
# <stage><dropboxwait> - If this element is present as well as <activebase>, specify a
#                        dropbox waiting interval.  Specify as floating point days.
#                        Create dataset "<activebase>_wait."
#
# <stage><prestagefraction> - This parameter should be a floating point number between
#                       0 and 1 (default 0).  If nonzero, the separate batch job that
#                       starts the sam project (if any) will prestage at least the
#                       specified fraction of files from the input sam project before
#                       exiting.
#
# <stage><ana>        - Analysis flag (0 or 1, default 0).  Setting this flag to 1
#                       informs project.py that this stage does not contain a RootOutput
#                       module, and not to expect any artroot output file.  This flag
#                       effectively converts command line action options to the
#                       analysis equivalent (e.g. --check acts like --checkana).
#
# <stage><numjobs> - Number of worker jobs (default 1).
# <stage><numevents> - Number of events (override project level number of events).
# <stage><maxfilesperjob> - Maximum number of files to deliver to a single job
#             Useful in case you want to limit output file size or keep
#             1 -> 1 correlation between input and output
# <stage><targetsize> - Specify target size for input files.  If specified,
#                       this attribute may override <numjobs> in the downward
#                       direction (i.e. <numjobs> is the maximum number of jobs).
# <stage><defname> - Sam output dataset defition name (default none).
# <stage><anadefname> - Sam analysis output dataset defition name (default none).
# <stage><datatier> - Sam data tier (default none).
# <stage><datastream> - Sam data stream (default none).
# <stage><anadatatier> - Sam analysis data tier (default none).
# <stage><anadatastream> - Sam analysis data stream (default none).
# <stage><submitscript> - Presubmission check script.  Must be on execution path.
#                         If this script exits with nonzero exit status, job submission
#                         is aborted.
# <stage><initscript> - Worker initialization script (condor_lar.sh --init-script).  Repeatable.
# <stage><initsource> - Worker initialization bash source script (condor_lar.sh --init-source).
# <stage><endscript>  - Worker finalization script (condor_lar.sh --end-script).  Repeatable.
# <stage><merge>  - Name of special histogram merging program or script (default "hadd -T",
#                   can be overridden at each stage).
#                   Set to "1" to generate merging metadata for artroot files.
# <stage><anamerge> - Set to "1" to generate merging metadata for analysis files.
# <stage><resource> - Jobsub resources (comma-separated list: DEDICATED,OPPORTUNISTIC,
#                     OFFSITE,FERMICLOUD,PAID_CLOUD,FERMICLOUD8G).
#                     Default: DEDICATED,OPPORTUNISTIC.
# <stage><lines>   - Arbitrary condor commands (expert option, jobsub_submit --lines=...).
# <stage><site>    - Specify sites (default jobsub decides).
# <stage><blacklist> - Blacklist sites (default jobsub decides).
# <stage><cpu>     - Number of cpus (jobsub_submit --cpu=...).
# <stage><disk>    - Amount of scratch disk space (jobsub_submit --disk=...).
#                    Specify value and unit (e.g. 50GB).
# <stage><memory>  - Specify amount of memory in MB (jobsub_submit --memory=...).
# <stage><script>  - Name of batch worker script (default condor_lar.sh).
#                    The batch script must be on the execution path.
# <stage><startscript> - Name of batch worker start project script (default condor_start_project.sh)
#                    Must be on execution path.
# <stage><stopscript> - Name of batch worker stop project script (default condor_start_project.sh)
#                    Must be on execution path.
# <stage><output>  - Specify output file name.  Can aslso appear in fcl substages (see below).
# <stage><datafiletypes>  - Specify file types that should be considered as data and
#                           saved in batch jobs (comma-separated list).  Default "root".
# <stage><TFileName>   - Ability to specify unique output TFile Name
#                        (Required when generating Metadata for TFiles)
# <stage><jobsub>  - Arbitrary jobsub_submit option(s).  Space-separated list.
#                    Only applies to main worker submission, not sam start/stop
#                    project submissions.
# <stage><jobsub_start>  - Arbitrary jobsub_submit option(s).  Space-separated list.
#                    Applies to sam start/stop project submissions.
# <stage><jobsub_timeout> - Jobsubmission timeout (seconds).
# <stage><maxfilesperjob> - Maximum number of files to be processed in a single worker.
# <stage><exe>     - Executable (default "lar").  Can also appear in fcl substages (see below).
# <stage><schema>  - Sam schema (default none).  Use "root" to stream using xrootd.
# <stage><check>   - Do on-node validation and sam declaration (0 or 1, default 0).
# <stage><copy>    - Copy validated root files to FTS (0 or 1, default 0).
# <stage><cvmfs>   - Cvmfs flag (0 or 1, default 1).  If nonzero, add option
#              "--append_condor_requirements='(TARGET.HAS_CVMFS_<experiment>_opensciencegrid_org==true)'"
# <stage><stash>   - Stash cache flag (0 or 1, default 1).  If nonzero, add option
#              "--append_condor_requirements='(TARGET.HAS_CVMFS_<experiment>_osgstorage_org==true)'"
# <stage><singularity> - Singularity flag (0 or 1, default 1).
#
# Batch job substages.
#
# Batch jobs can have multiple substages.  The number of substages equals the number
# of <fcl> elements.  Each <fcl> element triggers the execution of a different executable
# within a single batch job.  Some aspects of the environment are tunable within each 
# substage by specifying additional subelements within each <fcl> slement.
#
# <stage><fcl> - Name of fcl file.  This should come first within each <fcl> element
#                before additional substage subelements.
# <stage><fcl><initsource> - Initialization source script for this substage.
# <stage><fcl><endstage> - Finalization script for this substage.
# <stage><fcl><exe> - Executable to use in this substage (default "lar").
# <stage><fcl><output> - Output file name for this substage.
# <stage><fcl><projectname> - Override project name for this substage.
# <stage><fcl><stagename> - Override stage name for this substage.
# <stage><fcl><version> - Override project version for this substage.
#
#
# <fcldir>  - Directory in which to search for fcl files (optional, repeatable).
#             Fcl files are searched for in the following directories, in order.
#             1.  Fcl directories specified using <fcldir>.
#             2.  $FHICL_FILE_PATH.
#             Regardless of where an fcl file is found, a copy is placed
#             in the work directory before job submission.  It is an
#             error of the fcl file isn't found.
#
######################################################################
#
# Bookkeeping Files
# -----------------
#
# Project.py uses bookkeeping files that it manages to record the state
# of project jobs.  These bookkeeping files are stored in the log
# directoroy of each project stage (XML element <logdir>).  Most of these
# files are gnerated or updated after a "check" operation (project.py --check).
#
# Here is a list of all bookkeeping files used by project.py:
#
# checked - A file with empty contents used to record the timestamp
#           of the latest check for this stage.
# files.list - A list of all good art output files (full paths), one file
#              per line.
# file_<data_stream>.list - A list of all goot art output files in the
#                           specified stream.
# events.list - A list of all good art output files (full paths), one
#               file per line, plus on the same line the number of events.
# filesana.list - A list of all good non-art output root files (full
#                 paths), one file per line.
# transferred_uris.list - A list of input files that were successfully
#                         processed.  This file is a concatenated version of
#                         "transferred_uris.list" files from successful
#                         process subdirectories.
# missing_files.list - A list of input files that were not successfully
#                      processeed.
# bad.list - A list of failed process subdirectories.
# sam_projects.list - A list of at sam projects from successful processes.
#                     This file is a concatenated and sorted version of
#                     "sam_project.txt" files from successful process
#                     subdirectories.
# cpids.list - A list of successful sam consumer process ids.  This file
#              is a concatenated version of "cpid.txt" files from
#              successful process subdirectories.
#
# jobids.list - A list of all submitted jobsub jobids.
#
######################################################################

from __future__ import absolute_import
from __future__ import print_function
import sys, os, stat, subprocess, shutil, json, getpass, uuid, tempfile, hashlib
try:
    import urllib.request as urlrequest
except ImportError:
    import urllib as urlrequest
import larbatch_posix
import threading
try:
    import queue
except ImportError:
    import Queue as queue
from xml.dom.minidom import parse
import project_utilities, root_metadata
from project_modules.projectdef import ProjectDef
from project_modules.projectstatus import ProjectStatus
from project_modules.batchstatus import BatchStatus
from project_modules.jobsuberror import JobsubError
from project_modules.ifdherror import IFDHError
import larbatch_utilities
from larbatch_utilities import convert_str
from larbatch_utilities import convert_bytes
import samweb_cli

samweb = None           # Initialized SAMWebClient object
extractor_dict = None   # Metadata extractor
proxy_ok = False

# Function to make sure global SAMWebClient object is initialized.
# Also imports extractor_dict module.
# This function should be called before using samweb.

def import_samweb():

    # Get intialized samweb, if not already done.

    global samweb
    global extractor_dict
    global expMetaData


    if samweb == None:
        samweb = project_utilities.samweb()
        from extractor_dict import expMetaData

# Multi-project clean function.

def docleanx(projects, projectname, stagename, clean_descendants = True):
    print(projectname, stagename)

    # Loop over projects and stages.
    # Clean all stages beginning with the specified project/stage.
    # For empty project/stage name, clean all stages.
    #
    # For safety, only clean directories if the uid of the
    # directory owner matches the current uid or effective uid.
    # Do this even if the delete operation is allowed by filesystem
    # permissions (directories may be group- or public-write
    # because of batch system).

    uid = os.getuid()
    euid = os.geteuid()
    cleaned_bookdirs = []

    # Clean iteratively.

    done_cleaning = False
    while not done_cleaning:

        cleaned_something = False

        # Loop over projects and stages.

        for project in projects:
            for stage in project.stages:

                clean_this_stage = False

                # Skip this stage if it has already been cleaned.

                if not stage.bookdir in cleaned_bookdirs:

                    # Determine if this is the first stage we want to clean.

                    if (projectname == '' or project.name == projectname) and \
                       (stagename == '' or stage.name == stagename):

                        clean_this_stage = True

                    # Determine if we want to clean this stage because it uses
                    # an input filelist that lives in an already-cleaned bookdir.

                    elif clean_descendants and stage.inputlist != '' and \
                         os.path.dirname(stage.inputlist) in cleaned_bookdirs:

                        clean_this_stage = True

                    # Do cleaning.

                    if clean_this_stage:
                        cleaned_something = True
                        cleaned_bookdirs.append(stage.bookdir)

                        print('Clean project %s, stage %s' % (project.name, stage.name))

                        # Clean this stage outdir.

                        if larbatch_posix.exists(stage.outdir):
                            dir_uid = larbatch_posix.stat(stage.outdir).st_uid
                            if dir_uid == uid or dir_uid == euid:
                                print('Clean directory %s.' % stage.outdir)
                                larbatch_posix.rmtree(stage.outdir)
                            else:
                                raise RuntimeError('Owner mismatch, delete %s manually.' % stage.outdir)

                        # Clean this stage logdir.

                        if larbatch_posix.exists(stage.logdir):
                            dir_uid = larbatch_posix.stat(stage.logdir).st_uid
                            if dir_uid == uid or dir_uid == euid:
                                print('Clean directory %s.' % stage.logdir)
                                larbatch_posix.rmtree(stage.logdir)
                            else:
                                raise RuntimeError('Owner mismatch, delete %s manually.' % stage.logdir)

                        # Clean this stage workdir.

                        if larbatch_posix.exists(stage.workdir):
                            dir_uid = larbatch_posix.stat(stage.workdir).st_uid
                            if dir_uid == uid or dir_uid == euid:
                                print('Clean directory %s.' % stage.workdir)
                                larbatch_posix.rmtree(stage.workdir)
                            else:
                                raise RuntimeError('Owner mismatch, delete %s manually.' % stage.workdir)

                        # Clean this stage bookdir.

                        if larbatch_posix.exists(stage.bookdir):
                            dir_uid = larbatch_posix.stat(stage.bookdir).st_uid
                            if dir_uid == uid or dir_uid == euid:
                                print('Clean directory %s.' % stage.bookdir)
                                larbatch_posix.rmtree(stage.bookdir)
                            else:
                                raise RuntimeError('Owner mismatch, delete %s manually.' % stage.bookdir)

        done_cleaning = not cleaned_something

    # Done.

    return

# Stage status fuction.

def dostatus(projects):

    # BatchStatus constructor requires authentication.

    project_utilities.test_kca()

    # For backward compatibility, allow this function to be called with
    # either a single project or a list of projects.

    prjs = projects
    if type(projects) != type([]) and type(projects) != type(()):
        prjs = [projects]

    project_status = ProjectStatus(prjs)
    batch_status = BatchStatus(prjs)

    for project in prjs:

        print('\nProject %s:' % project.name)

        # Loop over stages.

        for stage in project.stages:

            stagename = stage.name
            stage_status = project_status.get_stage_status(stagename)
            b_stage_status = batch_status.get_stage_status(stagename)
            if stage_status.exists:
                print('\nStage %s: %d art files, %d events, %d analysis files, %d errors, %d missing files.' % (
                    stagename, stage_status.nfile, stage_status.nev, stage_status.nana,
                    stage_status.nerror, stage_status.nmiss))
            else:
                print('\nStage %s output directory does not exist.' % stagename)
            print('Stage %s batch jobs: %d idle, %d running, %d held, %d other.' % (
                stagename, b_stage_status[0], b_stage_status[1], b_stage_status[2], b_stage_status[3]))
    return


# Recursively extract projects from an xml element.

def find_projects(element, check=True):

    projects = []

    # First check if the input element is a project.  In that case, return a
    # list containing the project name as the single element of the list.

    if element.nodeName == 'project':
        default_input_by_stage = {}
        project = ProjectDef(element, '', default_input_by_stage, check=check)
        projects.append(project)

    else:

        # Input element is not a project.
        # Loop over subelements.

        default_input = ''
        default_input_by_stage = {}
        subelements = element.getElementsByTagName('project')
        for subelement in subelements:
            project = ProjectDef(subelement, default_input, default_input_by_stage, check=check)
            projects.append(project)
            for stage in project.stages:
                stage_list = os.path.join(stage.bookdir, 'files.list')
                default_input_by_stage[stage.name] = stage_list
                default_input = stage_list

    # Done.

    return projects


# Extract all projects from the specified xml file.

def get_projects(xmlfile, check=True):

    # Cache results.

    if xmlfile in get_projects.cache:
        return get_projects.cache[xmlfile]

    # Parse xml (returns xml document).

    if xmlfile == '-':
        xml = sys.stdin
    elif xmlfile.find(':') < 0:
        xml = open(xmlfile)
    else:
        xml = urlrequest.urlopen(xmlfile)
    doc = parse(xml)

    # Extract root element.

    root = doc.documentElement

    # Find project names in the root element.

    projects = find_projects(root, check=check)

    # Cache result.

    get_projects.cache[xmlfile] = projects

    # Done.

    return projects

# Get_projects result cache.

get_projects.cache = {}


# Select the specified project.

def select_project(projects, projectname, stagename):

    for project in projects:
        if projectname == '' or projectname == project.name:
            for stage in project.stages:
                if stagename == '' or stagename == stage.name:
                    return project

    # Failure if we fall out of the loop.

    return None


# Extract the specified project element from xml file.

def get_project(xmlfile, projectname='', stagename='', check=True):
    projects = get_projects(xmlfile, check=check)
    project = select_project(projects, projectname, stagename)
    return project

# Extract the next sequential stage

def next_stage(projects, stagename, circular=False):

    # Loop over projects.

    found = False
    for project in projects:

        # Loop over stages.

        for stage in project.stages:
            if found:
                return stage
            if stage.name == stagename:
                found = True

    # Circular mode: Choose first stage if we fell out of the loop.

    if circular and len(projects) > 0 and len(projects[0].stages) > 0:
        return projects[0].stages[0]

    # Finally return None if we didn't find anything appropriate.

    return None

# Extract the previous sequential stage.

def previous_stage(projects, stagename, circular=False):

    # Initialize result None or last stage (if circular).

    result = None
    if circular and len(projects) > 0 and len(projects[-1].stages) > 0:
        result = projects[-1].stages[-1]

    # Loop over projects.

    for project in projects:

        # Loop over stages.

        for stage in project.stages:
            if stage.name == stagename:
                return result
            result = stage

    # Return default answer if we fell out of the loop.

    return result

# Extract pubsified stage from xml file.
# Return value is a 2-tuple (project, stage).

def get_pubs_stage(xmlfile, projectname, stagename, run, subruns, version=None):
    projects = get_projects(xmlfile)
    project = select_project(projects, projectname, stagename)
    if project == None:
        raise RuntimeError('No project selected for projectname=%s, stagename=%s' % (
            projectname, stagename))
    stage = project.get_stage(stagename)
    if stage == None:
        raise RuntimeError('No stage selected for projectname=%s, stagename=%s' % (
            projectname, stagename))
    get_projects.cache = {}
    stage.pubsify_input(run, subruns, version)
    stage.pubsify_output(run, subruns, version)
    get_projects.cache = {}
    return project, stage


# Check a single root file.
# Returns a 2-tuple containing the number of events and stream name.
# The number of events conveys the following information:
# 1.  Number of events (>=0) in TTree named "Events."
# 2.  -1 if root file does not contain an Events TTree, but is otherwise valid (openable).
# 3.  -2 for error (root file does not exist or is not openable).

def check_root_file(path, logdir):

    global proxy_ok
    result = (-2, '')
    json_ok = False
    md = []

    # First check if root file exists (error if not).

    if not larbatch_posix.exists(path):
        return result

    # See if we have precalculated metadata for this root file.

    json_path = os.path.join(logdir, os.path.basename(path) + '.json')
    if larbatch_posix.exists(json_path):

        # Get number of events from precalculated metadata.

        try:
            lines = larbatch_posix.readlines(json_path)
            s = ''
            for line in lines:
                s = s + line

            # Convert json string to python dictionary.

            md = json.loads(s)

            # If we get this far, say the file was at least openable.

            result = (-1, '')

            # Extract number of events and stream name from metadata.

            if len(list(md.keys())) > 0:
                nevroot = -1
                stream = ''
                if 'events' in md:
                    nevroot = int(md['events'])
                if 'data_stream' in md:
                    stream = md['data_stream']
                result = (nevroot, stream)
            json_ok = True
        except:
            result = (-2, '')
    return result


# Check data files in the specified directory.

def check_root(outdir, logdir, data_file_types):

    # This method looks for files with file types matching data_file_types.
    # If such files are found, it also checks for the existence of
    # an Events TTree.
    #
    # Returns a 3-tuple containing the following information.
    # 1.  Total number of events in art root files.
    # 2.  A list of 3-tuples with an entry for each art root file.
    #     The 3-tuple contains the following information.
    #     a) Filename (full path).
    #     b) Number of events
    #     c) Stream name.
    # 3.  A list of histogram root files.

    nev = -1
    roots = []
    hists = []

    print('Checking root files in directory %s.' % outdir)
    filenames = larbatch_posix.listdir(outdir)
    for filename in filenames:
        name, ext = os.path.splitext(filename)
        if len(ext) > 0 and ext[1:] in data_file_types:
            path = os.path.join(outdir, filename)
            nevroot, stream = check_root_file(path, logdir)
            if nevroot >= 0:
                if nev < 0:
                    nev = 0
                nev = nev + nevroot
                roots.append((os.path.join(outdir, filename), nevroot, stream))

            elif nevroot == -1:

                # Valid data file, not an art root file.

                hists.append(os.path.join(outdir, filename))

            else:

                # Found a .root file that is not openable.
                # Print a warning, but don't trigger any other error.

                print('Warning: File %s in directory %s is not a valid root file.' % (filename, outdir))

    # Done.

    return (nev, roots, hists)


# Get the list of input files for a project stage.

def get_input_files(stage):

    # In case of single file or file list input, files are returned exactly
    # as specified, which would normallly be as the full path.
    # In case of sam input, only the file names are returned (guaranteed unique).

    result = []
    if stage.inputfile != '':
        result.append(stage.inputfile)

    elif stage.inputlist != '' and larbatch_posix.exists(stage.inputlist):
        try:
            input_filenames = larbatch_posix.readlines(stage.inputlist)
            for line in input_filenames:
                words = line.split()
                result.append(words[0])
        except:
            pass

    elif stage.inputdef != '':
        import_samweb()
        result = samweb.listFiles(defname=stage.inputdef)

    # Done.

    return result

# Shorten root file names to have fewer than 200 characters.

def doshorten(stage):

    # Untar log files.

    untarlog(stage)

    # Loop over .root files in outdir.

    for out_subpath, subdirs, files in larbatch_posix.walk(stage.outdir):

        # Only examine files in leaf directories.

        if len(subdirs) != 0:
            continue

        subdir = os.path.relpath(out_subpath, stage.outdir)
        log_subpath = os.path.join(stage.bookdir, subdir)

        for file in files:
            if file[-5:] == '.root':
                if len(file) >= 200:

                    # Long filenames renamed here.

                    file_path = os.path.join(out_subpath, file)
                    shortfile = file[:150] + str(uuid.uuid4()) + '.root'
                    shortfile_path = os.path.join(out_subpath, shortfile)
                    print('%s\n->%s\n' % (file_path, shortfile_path))
                    larbatch_posix.rename(file_path, shortfile_path)

                    # Also rename corresponding json file, if it exists.

                    json_path = os.path.join(log_subpath, file + '.json')
                    if larbatch_posix.exists(json_path):
                        shortjson = shortfile + '.json'
                        shortjson_path = os.path.join(log_subpath, shortjson)
                        print('%s\n->%s\n' % (json_path, shortjson_path))
                        larbatch_posix.rename(json_path, shortjson_path)

    return

# Untar tarred up log files in logtir into bookdir.

def untarlog(stage):

    # Walk over logdir to look for log files.

    for log_subpath, subdirs, files in larbatch_posix.walk(stage.logdir):

        # Only examine leaf directories.

        if len(subdirs) != 0:
            continue
        subdir = os.path.relpath(log_subpath, stage.logdir)
        if subdir == '.':
            continue
        book_subpath = os.path.join(stage.bookdir, subdir)
        for file in files:
            if file.startswith('log') and file.endswith('.tar'):
                src = '%s/%s' % (log_subpath, file)
                dst = '%s/%s' % (book_subpath, file)
                flag = '%s.done' % dst

                # Decide if we need to copy this tarball to bookdir.

                if dst != src and not larbatch_posix.exists(flag):

                    # Copy tarball to bookdir.

                    print('Copying tarball %s into %s' % (src, book_subpath))
                    if not larbatch_posix.isdir(book_subpath):
                        larbatch_posix.makedirs(book_subpath)
                    larbatch_posix.copy(src, dst)

                # Decide if we need to extract this tarball into bookdir.

                if not larbatch_posix.exists(flag):

                    # Extract tarball.

                    print('Extracting tarball %s' % dst)
                    jobinfo = subprocess.Popen(['tar','-xf', dst, '-C', book_subpath,
                                                '--exclude=beam*.dat',
                                                '--exclude=beam*.info',
                                                '--exclude=core*',
                                                '--exclude=*.db',
                                                '--exclude=*.sh',
                                                '--exclude=*.py*',
                                                '--exclude=*.tar'],
                                               stdout=subprocess.PIPE,
                                               stderr=subprocess.PIPE)
                    jobout, joberr = jobinfo.communicate()
                    jobout = convert_str(jobout)
                    joberr = convert_str(joberr)
                    rc = jobinfo.poll()
                    if rc != 0:
                        print(jobout)
                        print(joberr)
                        print('Failed to extract log tarball in %s' % dst)

                    else:

                        # Create flag file.

                        f = larbatch_posix.open(flag, 'w')
                        f.write('\n')         # Don't want zero size file.
                        f.close()

                        # Delete copy of tarball.

                        if dst != src:
                            larbatch_posix.remove(dst)

    return

# Check project results in the specified directory.

def docheck(project, stage, ana, quick=False):

    # This method performs various checks on worker subdirectories, named
    # as <cluster>_<process>, where <cluster> and <process> are integers.
    # In contrast, sam start and stop project jobs are named as
    # <cluster>_start and <cluster>_stop.
    #
    # Return 0 if all checks are OK, meaning:
    # a) No errors detected for any process.
    # b) At least one good root file (if not ana).
    # Otherwise return nonzero.
    #
    # The following checks are performed.
    #
    # 1.  Make sure subdirectory names are as expected.
    #
    # 2.  Look for at least one art root file in each worker subdirectory
    #     containing a valid Events TTree.  Complain about any
    #     that do not contain such a root file.
    #
    # 3.  Check that the number of events in the Events tree are as expected.
    #
    # 4.  Complain about any duplicated art root file names (if sam metadata is defined).
    #
    # 5.  Check job exit status (saved in lar.stat).
    #
    # 6.  For sam input, make sure that files sam_project.txt and cpid.txt are present.
    #
    # 7.  Check that any non-art root files are openable.
    #
    # 8.  Make sure file names do not exceed 200 characters (if sam metadata is defined).
    #
    # In analysis mode (if argumment ana != 0), skip checks 2-4, but still do
    # checks 1 and 5-7.
    #
    # This function also creates the following files in the specified directory.
    #
    # 1.  files.list  - List of good root files.
    # 2.  events.list - List of good root files and number of events in each file.
    # 3.  bad.list    - List of worker subdirectories with problems.
    # 4.  missing_files.list - List of unprocessed input files.
    # 5.  sam_projects.list - List of successful sam projects.
    # 6.  cpids.list        - list of successful consumer process ids.
    # 7.  filesana.list  - List of non-art root files (histograms and/or ntuples).
    #
    # For projects with no input (i.e. generator jobs), if there are fewer than
    # the requisite number of good generator jobs, a "missing_files.list" will be
    # generated with lines containing /dev/null.

    # Untar log files into bookdir.

    untarlog(stage)

    # Quick check?

    if quick == 1 and not ana:
        return doquickcheck(project, stage, ana)

    stage.checkinput()

    # Check that output and log directories exist.

    if not larbatch_posix.exists(stage.outdir):
        print('Output directory %s does not exist.' % stage.outdir)
        return 1
    if not larbatch_posix.exists(stage.bookdir):
        print('Log directory %s does not exist.' % stage.bookdir)
        return 1

    import_samweb()
    has_metadata = project.file_type != '' or project.run_type != ''
    has_input = stage.inputfile != '' or stage.inputlist != '' or stage.inputdef != ''
    print('Checking directory %s' % stage.bookdir)

    # Count total number of events and root files.

    nev_tot = 0
    nroot_tot = 0

    # Loop over subdirectories (ignore files and directories named *_start and *_stop).

    procmap = {}      # procmap[subdir] = <list of art root files and event counts>
    processes = []    # Integer process numbers derived from subdirectory names.
    filesana = []     # List of non-art root files.
    sam_projects = [] # List of sam projects.
    cpids = []        # List of successful sam consumer process ids.
    uris = []         # List of input files processed successfully.
    bad_workers = []  # List of bad worker subdirectories.


    for log_subpath, subdirs, files in larbatch_posix.walk(stage.bookdir):

        # Only examine files in leaf directories.

        if len(subdirs) != 0:
            continue

        subdir = os.path.relpath(log_subpath, stage.bookdir)
        if subdir == '.':
            continue
        out_subpath = os.path.join(stage.outdir, subdir)
        dirok = project_utilities.fast_isdir(log_subpath)

        # Update list of sam projects from start job.

        if dirok and log_subpath[-6:] == '_start':
            filename = os.path.join(log_subpath, 'sam_project.txt')
            if larbatch_posix.exists(filename):
                sam_project = larbatch_posix.readlines(filename)[0].strip()
                if sam_project != '' and not sam_project in sam_projects:
                    sam_projects.append(sam_project)

        # Regular worker jobs checked here.

        if dirok and not subdir[-6:] == '_start' and not subdir[-5:] == '_stop' \
                and not subdir == 'log':

            bad = 0

            # Make sure that corresponding output directory exists.

            if not project_utilities.fast_isdir(out_subpath):
                print('No output directory corresponding to subdirectory %s.' % subdir)
                bad = 1

            # Check lar exit status (if any).

            if not bad:
                stat_filename = os.path.join(log_subpath, 'lar.stat')
                if larbatch_posix.exists(stat_filename):
                    status = 0
                    try:
                        status = int(larbatch_posix.readlines(stat_filename)[0].strip())
                        if status != 0:
                            print('Job in subdirectory %s ended with non-zero exit status %d.' % (
                                subdir, status))
                            bad = 1
                    except:
                        print('Bad file lar.stat in subdirectory %s.' % subdir)
                        bad = 1

            # Now check root files in this subdirectory.

            if not bad:
                nev = 0
                roots = []
                nev, roots, subhists = check_root(out_subpath, log_subpath, stage.datafiletypes)
                if not ana:
                    if len(roots) == 0 or nev < 0:
                        print('Problem with root file(s) in subdirectory %s.' % subdir)
                        bad = 1
                elif nev < -1 or len(subhists) == 0:
                    print('Problem with analysis root file(s) in subdirectory %s.' % subdir)
                    bad = 1


            # Check for duplicate filenames (only if metadata is being generated).

            if not bad and has_metadata:
                for root in roots:
                    rootname = os.path.basename(root[0])
                    for s in list(procmap.keys()):
                        oldroots = procmap[s]
                        for oldroot in oldroots:
                            oldrootname = os.path.basename(oldroot[0])
                            if rootname == oldrootname:
                                print('Duplicate filename %s in subdirectory %s' % (rootname,
                                                                                    subdir))
                                olddir = os.path.basename(os.path.dirname(oldroot[0]))
                                print('Previous subdirectory %s' % olddir)
                                bad = 1

            # Make sure root file names do not exceed 200 characters.

            if not bad and has_metadata:
                for root in roots:
                    rootname = os.path.basename(root[0])
                    if len(rootname) >= 200:
                        print('Filename %s in subdirectory %s is longer than 200 characters.' % (
                            rootname, subdir))
                        bad = 1

            # Check existence of sam_project.txt and cpid.txt.
            # Update sam_projects and cpids.

            if not bad and stage.inputdef != '':
                filename1 = os.path.join(log_subpath, 'sam_project.txt')
                if not larbatch_posix.exists(filename1):
                    print('Could not find file sam_project.txt')
                    bad = 1
                filename2 = os.path.join(log_subpath, 'cpid.txt')
                if not larbatch_posix.exists(filename2):
                    print('Could not find file cpid.txt')
                    bad = 1
                if not bad:
                    sam_project = larbatch_posix.readlines(filename1)[0].strip()
                    if not sam_project in sam_projects:
                        sam_projects.append(sam_project)
                    cpid = larbatch_posix.readlines(filename2)[0].strip()
                    if not cpid in cpids:
                        cpids.append(cpid)

            # Check existence of transferred_uris.list.
            # Update list of uris.

            if not bad and (stage.inputlist !='' or stage.inputfile != ''):
                filename = os.path.join(log_subpath, 'transferred_uris.list')
                if not larbatch_posix.exists(filename):
                    print('Could not find file transferred_uris.list')
                    bad = 1
                if not bad:
                    lines = larbatch_posix.readlines(filename)
                    for line in lines:
                        uri = line.strip()
                        if uri != '':
                            uris.append(uri)

            # Save process number, and check for duplicate process numbers
            # (only if no input).

            if not has_input:
                subdir_split = subdir.split('_')
                if len(subdir_split) > 1:
                    process = int(subdir_split[1])
                    if process in processes:
                        print('Duplicate process number')
                        bad = 1
                    else:
                        processes.append(process)

            # Save information about good root files.

            if not bad:
                procmap[subdir] = roots

                # Save good histogram files.

                filesana.extend(subhists)

                # Count good events and root files.

                nev_tot = nev_tot + nev
                nroot_tot = nroot_tot + len(roots)

            # Update list of bad workers.

            if bad:
                bad_workers.append(subdir)

            # Print/save result of checks for one subdirectory.

            if bad:
                print('Bad subdirectory %s.' % subdir)

    # Done looping over subdirectoryes.
    # Dictionary procmap now contains a list of good processes
    # and root files.

    # Before attempting to create bookkeeping files in stage.bookdir, check
    # whether this directory is readable.  If not readable, return error
    # status without creating any bookkeeping files.  This is to prevent
    # hangs.

    contents = larbatch_posix.listdir(stage.bookdir)
    if len(contents) == 0:
        print('Directory %s may be dead.' % stage.bookdir)
        print('Returning error status without creating any bookkeeping files.')
        return 1

    # Open files.

    filelistname = os.path.join(stage.bookdir, 'files.list')
    filelist = safeopen(filelistname)

    eventslistname = os.path.join(stage.bookdir, 'events.list')
    eventslist = safeopen(eventslistname)

    badfilename = os.path.join(stage.bookdir, 'bad.list')
    badfile = safeopen(badfilename)

    missingfilesname = os.path.join(stage.bookdir, 'missing_files.list')
    missingfiles = safeopen(missingfilesname)

    filesanalistname = os.path.join(stage.bookdir, 'filesana.list')
    filesanalist = safeopen(filesanalistname)

    urislistname = os.path.join(stage.bookdir, 'transferred_uris.list')
    urislist = safeopen(urislistname)

    # Generate "files.list" and "events.list."
    # Also fill stream-specific file list.

    nproc = 0
    streams = {}    # {stream: file}
    nfile = 0
    for s in list(procmap.keys()):
        nproc = nproc + 1
        for root in procmap[s]:
            nfile = nfile + 1
            filelist.write('%s\n' % root[0])
            eventslist.write('%s %d\n' % root[:2])
            stream = root[2]
            if stream != '':
                if stream not in streams:
                    streamlistname = os.path.join(stage.bookdir, 'files_%s.list' % stream)
                    streams[stream] = safeopen(streamlistname)
                streams[stream].write('%s\n' % root[0])

    # Generate "bad.list"

    nerror = 0
    for bad_worker in bad_workers:
        badfile.write('%s\n' % bad_worker)
        nerror = nerror + 1

    # Generate "missing_files.list."

    nmiss = 0
    if stage.inputdef == '' and not stage.pubs_output:
        input_files = get_input_files(stage)
        if len(input_files) > 0:
            missing_files = list(set(input_files) - set(uris))
            for missing_file in missing_files:
                missingfiles.write('%s\n' % missing_file)
                nmiss = nmiss + 1
        else:
            nmiss = stage.num_jobs - len(procmap)
            for n in range(nmiss):
                missingfiles.write('/dev/null\n')


    # Generate "filesana.list."

    for hist in filesana:
        filesanalist.write('%s\n' % hist)

    # Generate "transferred_uris.list."

    for uri in uris:
        urislist.write('%s\n' % uri)

    # Print summary.

    if ana:
        print("%d processes completed successfully." % nproc)
        print("%d total good histogram files." % len(filesana))
    else:
        print("%d total good events." % nev_tot)
        print("%d total good root files." % nroot_tot)
        print("%d total good histogram files." % len(filesana))

    # Close files.

    filelist.close()
    if nfile == 0:
        project_utilities.addLayerTwo(filelistname)
    eventslist.close()
    if nfile == 0:
        project_utilities.addLayerTwo(eventslistname)
    if nerror == 0:
        badfile.write('\n')
    badfile.close()
    if nmiss == 0:
        missingfiles.write('\n')
    missingfiles.close()
    filesanalist.close()
    if len(filesana) == 0:
        project_utilities.addLayerTwo(filesanalistname)
    if len(uris) == 0:
        urislist.write('\n')
    urislist.close()
    for stream in list(streams.keys()):
        streams[stream].close()

    # Make sam files.

    if stage.inputdef != '' and not stage.pubs_input:

        # List of successful sam projects.

        sam_projects_filename = os.path.join(stage.bookdir, 'sam_projects.list')
        sam_projects_file = safeopen(sam_projects_filename)
        for sam_project in sam_projects:
            sam_projects_file.write('%s\n' % sam_project)
        sam_projects_file.close()
        if len(sam_projects) == 0:
            project_utilities.addLayerTwo(sam_projects_filename)

        # List of successfull consumer process ids.

        cpids_filename = os.path.join(stage.bookdir, 'cpids.list')
        cpids_file = safeopen(cpids_filename)
        for cpid in cpids:
            cpids_file.write('%s\n' % cpid)
        cpids_file.close()
        if len(cpids) == 0:
            project_utilities.addLayerTwo(cpids_filename)

        # Get number of consumed files.

        cpids_list = ''
        sep = ''
        for cpid in cpids:
            cpids_list = cpids_list + '%s%s' % (sep, cpid)
            sep = ','
        if cpids_list != '':
            dim = 'consumer_process_id %s and consumed_status consumed' % cpids_list
            import_samweb()
            nconsumed = samweb.countFiles(dimensions=dim)
        else:
            nconsumed = 0

        # Get number of unconsumed files.

        if cpids_list != '':
            udim = '(defname: %s) minus (%s)' % (stage.inputdef, dim)
        else:
            udim = 'defname: %s' % stage.inputdef
        nunconsumed = samweb.countFiles(dimensions=udim)
        nerror = nerror + nunconsumed

        # Sam summary.

        print('%d sam projects.' % len(sam_projects))
        print('%d successful consumer process ids.' % len(cpids))
        print('%d files consumed.' % nconsumed)
        print('%d files not consumed.' % nunconsumed)

        # Check project statuses.

        for sam_project in sam_projects:
            print('\nChecking sam project %s' % sam_project)
            import_samweb()
            url = samweb.findProject(sam_project, project_utilities.get_experiment())
            if url != '':
                result = samweb.projectSummary(url)
                nd = 0
                nc = 0
                nf = 0
                nproc = 0
                nact = 0
                if 'processes' in result:
                    processes = result['processes']
                    for process in processes:
                        nproc = nproc + 1
                        if 'status' in process:
                            if process['status'] == 'active':
                                nact = nact + 1
                        if 'counts' in process:
                            counts = process['counts']
                            if 'delivered' in counts:
                                nd = nd + counts['delivered']
                            if 'consumed' in counts:
                                nc = nc + counts['consumed']
                            if 'failed' in counts:
                                nf = nf + counts['failed']
                print('Status: %s' % result['project_status'])
                print('%d total processes' % nproc)
                print('%d active processes' % nact)
                print('%d files in snapshot' % result['files_in_snapshot'])
                print('%d files delivered' % (nd + nc))
                print('%d files consumed' % nc)
                print('%d files failed' % nf)
                print()

    # Done

    checkfilename = os.path.join(stage.bookdir, 'checked')
    checkfile = safeopen(checkfilename)
    checkfile.write('\n')
    checkfile.close()
    project_utilities.addLayerTwo(checkfilename)

    if stage.inputdef == '' or stage.pubs_input:
        print('%d processes with errors.' % nerror)
        print('%d missing files.' % nmiss)
    else:
        print('%d unconsumed files.' % nerror)

    # Return error status if any error or not good root file produced.
    # Also return error if no successful processes were detected

    result = 0
    if nerror != 0:
        result = 1
    if not ana and nroot_tot == 0:
        result = 1
    if len(procmap) == 0:
        result = 1
    return result

def doquickcheck(project, stage, ana):

    # Check that output and log directories exist. Dirs could be lost due to ifdhcp failures
    if not larbatch_posix.isdir(stage.outdir):
        print('Output directory %s does not exist.' % stage.outdir)
        return 1

    if not larbatch_posix.isdir(stage.bookdir):
        print('Log directory %s does not exist.' % stage.bookdir)
        return 1

    print('Checking directory %s' % stage.bookdir)

    #Aggregate the .list files form the bookdir up one dir. This is where the old docheck would put them, and it double-checks that the files made it back from the worker node.

    goodFiles        = []       # list of art root files
    goodAnaFiles     = []       # list of analysis root files
    eventLists       = []       # list of art root files and number of events
    badLists         = []       # list of bad root files
    anaFiles         = []       # list of ana files
    transferredFiles = []       # list of transferred files
    streamLists      = {}       # dictionary which keeps track of files per stream

    sam_projects     = []      # list of sam projects
    cpids            = []      # list of consumer process ids

    goodLogDirs      = set()   # Set of log directories.
    nErrors = 0                # Number of erors uncovered

    for log_subpath, subdirs, files in larbatch_posix.walk(stage.bookdir):

        # Only examine files in leaf directories.

        if len(subdirs) != 0:
            continue

        #skip start and stop project jobs for now
        if log_subpath[-6:] == '_start' or log_subpath[-5:] == '_stop':
            filename = os.path.join(log_subpath, 'sam_project.txt')
            if larbatch_posix.exists(filename):
                sam_project = larbatch_posix.readlines(filename)[0].strip()
                if sam_project != '' and not sam_project in sam_projects:
                    sam_projects.append(sam_project)
            continue


        print('Doing quick check of directory %s.' % log_subpath)

        subdir = os.path.relpath(log_subpath, stage.bookdir)

        out_subpath = os.path.join(stage.outdir, subdir)
        dirok = project_utilities.fast_isdir(log_subpath)

        #first check the missing_file.list


        validateOK = 1

        missingfilesname = os.path.join(log_subpath, 'missing_files.list')

        #print missingfilesname

        try:
            #print 'Reading %s' % missingfilesname
            missingfiles = project_utilities.saferead(missingfilesname)
        #if we can't find missing_files the check will not work
        except:
            print('Cannot open file: %s' % missingfilesname)
            validateOK = 0


        if validateOK == 1 and len(missingfiles) == 0:
            print('%s exists, but is empty' % missingfilesname)
            validateOK = 0


        if validateOK == 1:
            line = missingfiles[0]
            line = line.strip('\n')
            if( int(line) != 0 ):
                validateOK = 0


        #If the validation failed, continue.
        if validateOK != 1:
            nErrors += 1
            continue

        #Copy files.
        #print 'Appending Files'

        # Check existence of sam_project.txt and cpid.txt.
        # Update sam_projects and cpids.

        if stage.inputdef != '':

            filename1 = os.path.join(log_subpath, 'sam_project.txt')
            if not larbatch_posix.exists(filename1):
                print('Could not find file sam_project.txt')
                nErrors += 1
            else:
                sam_project = larbatch_posix.readlines(filename1)[0].strip()
                if not sam_project in sam_projects:
                    sam_projects.append(sam_project)

            filename2 = os.path.join(log_subpath, 'cpid.txt')
            if not larbatch_posix.exists(filename2):
                print('Could not find file cpid.txt')
                nErrors += 1
            else:
                cpid = larbatch_posix.readlines(filename2)[0].strip()
                if not cpid in cpids:
                    cpids.append(cpid)

        filelistsrc = os.path.join(log_subpath, 'files.list')
        tmpArray = scan_file(filelistsrc)

        if( tmpArray == [ -1 ] ):
            nErrors += 1
        else:
            goodFiles.extend(tmpArray)

        fileanalistsrc = os.path.join(log_subpath, 'filesana.list')
        tmpArray = scan_file(fileanalistsrc)

        if( not tmpArray == [ -1 ] ):
            goodAnaFiles.extend(tmpArray)

        eventlistsrc = os.path.join(log_subpath, 'events.list')

        tmpArray = scan_file(eventlistsrc)

        if( tmpArray == [ -1 ] ):
            nErrors += 1
        else:
            eventLists.extend(tmpArray)


        badfilesrc = os.path.join(log_subpath, 'bad.list')


        tmpArray = scan_file(badfilesrc)

        #bad list begin empty is okay
        if( tmpArray == [ -1 ] ):
            pass
        else:
            badLists.extend(tmpArray)

        '''
        missingfilesrc  = os.path.join(log_subpath, 'missing_files.list')

        tmpArray = scan_file(missingfilesrc)

        if( tmpArray == [ -1 ] ):
            nErrors += 1
        else:
            missingLists.extend(tmpArray)
        '''

        #if ana:
        #    filesanalistsrc = os.path.join(log_subpath, 'filesana.list')

        #    tmpArray = scan_file(filesanalistsrc)

        #    if( tmpArray == [ -1 ] ):
        #        nErrors += 1
        #    else:
        #        anaFiles.extend(tmpArray)

        urislistsrc = os.path.join(log_subpath, 'transferred_uris.list')

        tmpArray = scan_file(urislistsrc)

        #empty uri file is not nessecary an error
        if( tmpArray == [ -1 ] ):
            pass
        else:
            transferredFiles.extend(tmpArray)
        #create a list of files_*.list files. These are outputs from specific streams
        streamList = larbatch_posix.listdir(log_subpath)

        for stream in streamList:
            if( stream[:6] != "files_" ):
                continue
            streamfilesrc = os.path.join(log_subpath, stream)
            #print stream
            tmpArray = scan_file(streamfilesrc)
            if( tmpArray == [ -1 ] ):
                nErrors += 1
            else:
                if(streamLists.get(stream, "empty") == "empty" ):
                    streamLists[stream] = tmpArray
                else:
                    streamLists[stream].extend(tmpArray)

        if validateOK == 1:
            goodLogDirs.add(log_subpath)

    checkfilename = os.path.join(stage.bookdir, 'checked')
    checkfile = safeopen(checkfilename)
    checkfile.write('\n')
    checkfile.close()

    #create the input files.list for the next stage
    filelistdest = os.path.join(stage.bookdir, 'files.list')
    if larbatch_posix.exists(filelistdest):
        #print 'Deleting %s' % filelistdest
        larbatch_posix.remove(filelistdest)
    if len(goodLogDirs) == 1:
        src = '%s/files.list' % goodLogDirs.copy().pop()
        #print 'Symlinking %s to %s' % (src, filelistdest)
        larbatch_posix.symlink(src, filelistdest)
    else:
        #print 'Aggregating files.list'
        inputList = safeopen(filelistdest)
        for goodFile in goodFiles:
            #print goodFile
            inputList.write("%s\n" % goodFile)
        inputList.close()
    if len(goodFiles) == 0:
        project_utilities.addLayerTwo(filelistdest)

    #create the aggregated filesana.list
    fileanalistdest = os.path.join(stage.bookdir, 'filesana.list')
    if larbatch_posix.exists(fileanalistdest):
        #print 'Deleting %s' % fileanalistdest
        larbatch_posix.remove(fileanalistdest)
    if len(goodLogDirs) == 1:
        src = '%s/filesana.list' % goodLogDirs.copy().pop()
        #print 'Symlinking %s to %s' % (src, fileanalistdest)
        larbatch_posix.symlink(src, fileanalistdest)
    else:
        #print 'Aggregating filesana.list'
        anaList = safeopen(fileanalistdest)
        for goodAnaFile in goodAnaFiles:
            #print goodAnaFile
            anaList.write("%s\n" % goodAnaFile)
        anaList.close()
        if len(goodAnaFiles) == 0:
            project_utilities.addLayerTwo(fileanalistdest)

    #create the events.list for the next step
    eventlistdest = os.path.join(stage.bookdir, 'events.list')
    if larbatch_posix.exists(eventlistdest):
        #print 'Deleting %s' % eventlistdest
        larbatch_posix.remove(eventlistdest)
    if len(goodLogDirs) == 1:
        src = '%s/events.list' % goodLogDirs.copy().pop()
        #print 'Symlinking %s to %s' % (src, eventlistdest)
        larbatch_posix.symlink(src, eventlistdest)
    else:
        #print 'Aggregating events.list'
        eventsOutList = safeopen(eventlistdest)
        for event in eventLists:
            #print event
            eventsOutList.write("%s\n" % event)
        eventsOutList.close()
        if len(eventLists) == 0:
            project_utilities.addLayerTwo(eventlistdest)

    #create the bad.list for makeup jobs
    if(len(badLists) > 0):
        badlistdest = os.path.join(stage.bookdir, 'bad.list')
        badOutList = safeopen(badlistdest)
        for bad in badLists:
            badOutList.write("%s\n" % bad)
        badOutList.close()
        #project_utilities.addLayerTwo(badlistdest)

    #create the missing_files.list for makeup jobs
    missing_files = []
    if stage.inputdef == '' and not stage.pubs_output:
        input_files = get_input_files(stage)
        if len(input_files) > 0:
            missing_files = list(set(input_files) - set(transferredFiles))

    if len(missing_files) > 0:
        missinglistdest = os.path.join(stage.bookdir, 'missing_files.list')
        missingOutList = safeopen(missinglistdest)
        for missing in missing_files:
            missingOutList.write("%s\n" % missing)
        missingOutList.close()
        #project_utilities.addLayerTwo(missingOutList)

    #create the transferred_uris for the next step
    urilistdest = os.path.join(stage.bookdir, 'transferred_uris.list')
    if larbatch_posix.exists(urilistdest):
        #print 'Deleting %s' % urilistdest
        larbatch_posix.remove(urilistdest)
    if len(goodLogDirs) == 1 and len(transferredFiles) > 0:
        src = '%s/transferred_uris.list' % goodLogDirs.copy().pop()
        #print 'Symlinking %s to %s' % (src, urilistdest)
        larbatch_posix.symlink(src, urilistdest)
    else:
        #print 'Aggregating transferred_uris.list'
        uriOutList  = safeopen(urilistdest)
        for uri in transferredFiles:
            #print event
            uriOutList.write("%s\n" % uri)
        uriOutList.close()
        if len(transferredFiles) == 0:
            project_utilities.addLayerTwo(urilistdest)

    if stage.inputdef != '':
        samprojectdest = os.path.join(stage.bookdir, 'sam_projects.list')
        if larbatch_posix.exists(samprojectdest):
            #print 'Deleting %s' % samprojectdest
            larbatch_posix.remove(samprojectdest)
        if len(goodLogDirs) == 1:
            src = '%s/sam_project.txt' % goodLogDirs.copy().pop()
            #print 'Symlinking %s to %s' % (src, samprojectdest)
            larbatch_posix.symlink(src, samprojectdest)
        else:
            #print 'Aggregating sam_projects.list'
            samprojectfile = safeopen(samprojectdest)
            for sam in sam_projects:
                samprojectfile.write("%s\n" % sam)
            samprojectfile.close()
            if len(sam_projects) == 0:
                project_utilities.addLayerTwo(samprojectdest)

        cpiddest = os.path.join(stage.bookdir, 'cpids.list')
        if larbatch_posix.exists(cpiddest):
            #print 'Deleting %s' % cpiddest
            larbatch_posix.remove(cpiddest)
        if len(goodLogDirs) == 1:
            src = '%s/cpid.txt' % goodLogDirs.copy().pop()
            #print 'Symlinking %s to %s' % (src, cpiddest)
            larbatch_posix.symlink(src, cpiddest)
        else:
            #print 'Aggregating cpids.list'
            cpidfile = safeopen(cpiddest)
            for cp in cpids:
                cpidfile.write("%s \n" % cp)
            cpidfile.close()
            if len(cpids) == 0:
                project_utilities.addLayerTwo(cpiddest)


    for stream in streamLists:
        streamdest = os.path.join(stage.bookdir, stream)
        if larbatch_posix.exists(streamdest):
            #print 'Deleting %s' % streamdest
            larbatch_posix.remove(streamdest)
        if len(goodLogDirs) == 1:
            src = '%s/%s' % (goodLogDirs.copy().pop(), stream)
            #print 'Symlinking %s to %s' % (src, streamdest)
            larbatch_posix.symlink(src, streamdest)
        else:
            #print 'Aggregating %s' % stream
            streamOutList = safeopen(streamdest)
            for line in streamLists[stream]:
                streamOutList.write("%s\n" % line)
            streamOutList.close()
            if len(streamLists[stream]) == 0:
                project_utilities.addLayerTwo(streamdest)





    print('Number of errors = %d' % nErrors)

    return nErrors

# Check project results in the specified directory.

def dofetchlog(project, stage):

    # This funciton fetches jobsub log files using command
    # jobsub_fetchlog.  Fetched log files are stored in a subdirectory
    # called "log" in the stage output directory.
    #
    # This function has uses an algorithm to determine the log file
    # job id that is based on the worker environment as recorded in
    # file "env.txt" as returned from any worker.  Therefore, at least
    # one worker must have completed (successfully or not) for this
    # function to succeed.

    stage.checkinput()
    stage.checkdirs()

    # Look for files called "env.txt" in any subdirectory of
    # stage.bookdir.

    logids = []
    for dirpath, dirnames, filenames in larbatch_posix.walk(stage.bookdir):
        for filename in filenames:
            if filename == 'env.txt':

                # Look for either environment variable:
                #
                # 1. JOBSUBPARENTJOBID
                # 2. JOBSUBJOBID
                #
                # In either case, construct the log file id by
                # changing the process number to zero.

                logid = ''
                envpath = os.path.join(dirpath, filename)
                vars = larbatch_posix.readlines(envpath)

                # JOBSUBPARENTJOBID

                for var in vars:
                    varsplit = var.split('=', 1)
                    name = varsplit[0].strip()
                    if name == 'JOBSUBPARENTJOBID':
                        logid = varsplit[1].strip()

                        # Fix up the log file id by changing the process
                        # number to zero.

                        logsplit = logid.split('@', 1)
                        cluster_process = logsplit[0]
                        server = logsplit[1]
                        cluster = cluster_process.split('.', 1)[0]
                        logid = cluster + '.0' + '@' + server
                        logids.append(logid)
                        break

                # JOBSUBJOBID

                if logid == '':
                    for var in vars:
                        varsplit = var.split('=', 1)
                        name = varsplit[0].strip()
                        if name == 'JOBSUBJOBID':
                            logid = varsplit[1].strip()

                            # Fix up the log file id by changing the process
                            # number to zero.

                            logsplit = logid.split('@', 1)
                            cluster_process = logsplit[0]
                            server = logsplit[1]
                            cluster = cluster_process.split('.', 1)[0]
                            logid = cluster + '.0' + '@' + server
                            logids.append(logid)
                            break

    # Process all of the log ids that we found.

    if len(logids) > 0:

        # Make a directory to receive log files.

        logdir = os.path.join(stage.bookdir, 'log')
        if larbatch_posix.exists(logdir):
            larbatch_posix.rmtree(logdir)
        larbatch_posix.mkdir(logdir)

        # Loop over log ids.

        for logid in set(logids):

            # Do the actual fetch.
            # Tarball is fetched into current directory, and unpacked
            # into log directory.

            print('Fetching log files for id %s' % logid)
            command = ['jobsub_fetchlog']
            if project.server != '-' and project.server != '':
                command.append('--jobsub-server=%s' % project.server)
            command.append('--jobid=%s' % logid)
            command.append('--dest-dir=%s' % logdir)
            jobinfo = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            jobout, joberr = jobinfo.communicate()
            jobout = convert_str(jobout)
            joberr = convert_str(joberr)
            rc = jobinfo.poll()
            if rc != 0:
                raise JobsubError(command, rc, jobout, joberr)

        return 0

    else:

        # Done (failure).
        # If we fall out of the loop, we didn't find any files called env.txt, or
        # they didn't contain the right environment variables we need.
        # In this case, the most likely explanation is that no workers have
        # completed yet.

        print('Failed to fetch log files.')
        return 1


# Check sam declarations.
# Return 0 if all files are declared or don't have internal metadata.
# Return nonzero if some files have metadata but are are not declared.

def docheck_declarations(logdir, outdir, declare, ana=False):

    # Default result success (all files declared).

    result = 0

    # Initialize samweb.

    import_samweb()

    # Loop over root files listed in files.list or filesana.list.

    roots = []
    listname = 'files.list'
    if ana:
        listname = 'filesana.list'
    fnlist = os.path.join(logdir, listname)
    if larbatch_posix.exists(fnlist):
        roots = larbatch_posix.readlines(fnlist)
    else:
        raise RuntimeError('No %s file found %s, run project.py --check' % (listname, fnlist))

    for root in roots:
        path = root.strip()
        fn = os.path.basename(path)
        dirpath = os.path.dirname(path)
        dirname = os.path.relpath(dirpath, outdir)

        # Check metadata

        has_metadata = False
        try:
            md = samweb.getMetadata(filenameorid=fn)
            has_metadata = True
        except samweb_cli.exceptions.FileNotFound:
            pass

        # Report or declare file.

        if has_metadata:
            print('Metadata OK: %s' % fn)
        else:
            if declare:
                print('Declaring: %s' % fn)
                jsonfile = os.path.join(logdir, os.path.join(dirname, fn)) + '.json'
                mdjson = {}
                if larbatch_posix.exists(jsonfile):
                    mdlines = larbatch_posix.readlines(jsonfile)
                    mdtext = ''
                    for line in mdlines:
                        mdtext = mdtext + line
                    try:
                        md = json.loads(mdtext)
                        mdjson = md
                    except:
                        pass
                md = {}
                if ana:
                    md = mdjson
                else:
                    expSpecificMetaData = expMetaData(os.environ['SAM_EXPERIMENT'],larbatch_posix.root_stream(path))
                    md = expSpecificMetaData.getmetadata(mdjson)
                if len(md) > 0:
                    project_utilities.test_kca()

                    # Make lack of parent files a nonfatal error.
                    # This should probably be removed at some point.

                    try:
                        samweb.declareFile(md=md)
                    except:
                        #if md.has_key('parents'):
                        #    del md['parents']
                        #    samweb.declareFile(md=md)
                        print('SAM declare failed.')
                        result = 1

                else:
                    print('No sam metadata found for %s.' % fn)
            else:
                print('Not declared: %s' % fn)
                result = 1

    return result

# Print summary of files returned by sam query.

def dotest_declarations(dim):

    # Initialize samweb.

    import_samweb()

    # Do query

    result = samweb.listFilesSummary(dimensions=dim)
    for key in list(result.keys()):
        print('%s: %s' % (key, result[key]))

    return 0

# Check sam dataset definition.
# Return 0 if dataset is defined or definition name is null.
# Return nonzero if dataset is not defined.

def docheck_definition(defname, dim, define):

    # Default rssult success.

    result = 0

    # Return success for null definition.

    if defname == '':
        return result

    # Initialize samweb.

    import_samweb()

    # See if this definition already exists.

    def_exists = False
    try:
        desc = samweb.descDefinition(defname=defname)
        def_exists = True
    except samweb_cli.exceptions.DefinitionNotFound:
        pass

    # Make report and maybe make definition.

    if def_exists:
        print('Definition already exists: %s' % defname)
    else:
        if define:
            print('Creating definition %s.' % defname)
            project_utilities.test_kca()
            samweb.createDefinition(defname=defname, dims=dim)
        else:
            result = 1
            print('Definition should be created: %s' % defname)

    return result

# Print summary of files returned by dataset definition.

def dotest_definition(defname):

    # Initialize samweb.

    import_samweb()

    # Do query

    result = samweb.listFilesSummary(defname=defname)
    for key in list(result.keys()):
        print('%s: %s' % (key, result[key]))

    return 0

# Delete sam dataset definition.

def doundefine(defname):

    if defname == '':
        return 1

    # Initialize samweb.

    import_samweb()

    # See if this definition already exists.

    def_exists = False
    try:
        desc = samweb.descDefinition(defname=defname)
        def_exists = True
    except samweb_cli.exceptions.DefinitionNotFound:
        pass

    # Make report and maybe make definition.

    if def_exists:
        print('Deleting definition: %s' % defname)
        project_utilities.test_kca()
        samweb.deleteDefinition(defname=defname)
    else:
        print('No such definition: %s' % defname)

    return 0

# Check disk locations.  Maybe add or remove locations.
# This method only generates output and returns zero.

def docheck_locations(dim, outdir, add, clean, remove, upload):

    if add:
        print('Adding disk locations.')
    elif clean:
        print('Cleaning disk locations.')
    elif remove:
        print('Removing disk locations.')
    elif upload:
        print('Uploading to FTS.')
    else:
        print('Checking disk locations.')

    # Initialize samweb.

    import_samweb()

    # Loop over files queried by dimension string.

    filelist = samweb.listFiles(dimensions=dim, stream=False)

    # Look for listed files on disk under outdir.

    disk_dict = {}
    for filename in filelist:
        disk_dict[filename] = []
    for out_subpath, subdirs, files in larbatch_posix.walk(outdir):

        # Only examine files in leaf directories.

        if len(subdirs) != 0:
            continue

        for fn in files:
            if fn in filelist:
                disk_dict[fn].append(out_subpath)

    # Check sam locations.

    for filename in filelist:
        disk_locs = disk_dict[filename]
        sam_locs = samweb.locateFile(filenameorid=filename)
        if len(sam_locs) == 0 and not upload:
            print('No location: %s' % filename)

        # Make a double loop over disk and sam locations, in order
        # to identify locations that should added.
        # Note that we ignore the node part of the sam location.

        locs_to_add = []
        for disk_loc in disk_locs:
            should_add = True
            for sam_loc in sam_locs:
                if sam_loc['location_type'] == 'disk':
                    if disk_loc == sam_loc['location'].split(':')[-1]:
                        should_add = False
                        break
            if should_add:
                locs_to_add.append(disk_loc)

        # Loop over sam locations, in order to identify locations
        # that should be removed.  Note that for this step, we don't
        # necessarily assume that we found the disk location
        # in the directory search above, rather check the existence
        # of the file directly.

        locs_to_remove = []
        for sam_loc in sam_locs:
            if sam_loc['location_type'] == 'disk':

                # If remove is specified, uncondiontally remove this location.

                if remove:
                    locs_to_remove.append(sam_loc['location'])

                # Otherwise, check if file exists.

                else:

                    # Split off the node, if any, from the location.

                    local_path = os.path.join(sam_loc['location'].split(':')[-1], filename)
                    if not larbatch_posix.exists(local_path):
                        locs_to_remove.append(sam_loc['location'])

        # Loop over sam locations and identify files that can be uploaded.
        # If this file has no disk locations, don't do anything (not an error).
        # In case we decide to upload this file, always upload from the first
        # disk location.

        locs_to_upload = {}    # locs_to_upload[disk-location] = dropbox-directory
        should_upload = False
        if upload and len(disk_locs) > 0:
            should_upload = True
            for sam_loc in sam_locs:
                if sam_loc['location_type'] == 'tape':
                    should_upload = False
                    break
            if should_upload:
                dropbox = project_utilities.get_dropbox(filename)
                if not larbatch_posix.exists(dropbox):
                    print('Making dropbox directory %s.' % dropbox)
                    larbatch_posix.makedirs(dropbox)
                locs_to_upload[disk_locs[0]] = dropbox

        # Report results and do the actual adding/removing/uploading.

        for loc in locs_to_add:
            node = project_utilities.get_bluearc_server()
            if loc[0:6] == '/pnfs/':
                node = project_utilities.get_dcache_server()
            loc = node + loc.split(':')[-1]
            if add:
                print('Adding location: %s.' % loc)
                project_utilities.test_kca()
                samweb.addFileLocation(filenameorid=filename, location=loc)
            elif not upload:
                print('Can add location: %s.' % loc)

        for loc in locs_to_remove:
            if clean or remove:
                print('Removing location: %s.' % loc)
                project_utilities.test_kca()
                samweb.removeFileLocation(filenameorid=filename, location=loc)
            elif not upload:
                print('Should remove location: %s.' % loc)

        for loc in list(locs_to_upload.keys()):
            dropbox = locs_to_upload[loc]

            # Make sure dropbox directory exists.

            if not larbatch_posix.isdir(dropbox):
                print('Dropbox directory %s does not exist.' % dropbox)
            else:

                # Test whether this file has already been copied to dropbox directory.

                dropbox_filename = os.path.join(dropbox, filename)
                if larbatch_posix.exists(dropbox_filename):
                    print('File %s already exists in dropbox %s.' % (filename, dropbox))
                else:

                    # Copy file to dropbox.

                    loc_filename = os.path.join(loc, filename)

                    # Decide whether to use a symlink or copy.

                    if project_utilities.mountpoint(loc_filename) == \
                            project_utilities.mountpoint(dropbox_filename):
                        print('Symlinking %s to dropbox directory %s.' % (filename, dropbox))
                        relpath = os.path.relpath(os.path.realpath(loc_filename), dropbox)
                        print('relpath=',relpath)
                        print('dropbox_filename=',dropbox_filename)
                        larbatch_posix.symlink(relpath, dropbox_filename)

                    else:
                        print('Copying %s to dropbox directory %s.' % (filename, dropbox))
                        larbatch_posix.copy(loc_filename, dropbox_filename)

    return 0

# Check tape locations.
# Return 0 if all files in sam have tape locations.
# Return nonzero if some files in sam don't have tape locations.

def docheck_tape(dim):

    # Default result success.

    result = 0

    # Initialize samweb.

    import_samweb()

    # Loop over files queried by dimension string.

    nbad = 0
    ntot = 0
    filelist = samweb.listFiles(dimensions=dim, stream=True)
    while 1:
        try:
            filename = next(filelist)
        except StopIteration:
            break

        # Got a filename.

        ntot = ntot + 1

        # Look for sam tape locations.

        is_on_tape = False
        sam_locs = samweb.locateFile(filenameorid=filename)
        for sam_loc in sam_locs:
            if sam_loc['location_type'] == 'tape':
                is_on_tape = True
                break

        if is_on_tape:
            print('On tape: %s' % filename)
        else:
            result = 1
            nbad = nbad + 1
            print('Not on tape: %s' % filename)

    print('%d files.' % ntot)
    print('%d files need to be store on tape.' % nbad)

    return result

# Copy files to workdir and issue jobsub submit command.
# Return jobsubid.
# Raise exception if jobsub_submit returns a nonzero status.

def dojobsub(project, stage, makeup, recur, dryrun):

    # Default return.

    jobid = ''

    # Process map, to be filled later if we need one.

    procmap = ''

    # Temporary directory where we will copy the batch script(s) and dag.

    tmpdir = tempfile.mkdtemp()

    # Temporary directory where we will copy files destined for stage.workdir.

    tmpworkdir = tempfile.mkdtemp()

    #we're going to let jobsub_submit copy the workdir contents for us
    #each file that would go into the workdir is going to be added with
    # '-f <input_file>' with the full path, it can be either BlueArc or /pnfs/uboone

    jobsub_workdir_files_args = []

    # If there is an input list, copy it to the work directory.

    input_list_name = ''
    if stage.inputlist != '':
        input_list_name = os.path.basename(stage.inputlist)
        work_list_name = os.path.join(tmpworkdir, input_list_name)
        if stage.inputlist != work_list_name:
            input_files = larbatch_posix.readlines(stage.inputlist)
            print('Making input list.')
            work_list = safeopen(work_list_name)
            for input_file in input_files:
                print('Adding input file %s' % input_file)
                work_list.write('%s\n' % input_file.strip())
            work_list.close()
            print('Done making input list.')

    # Now locate the fcl file on the fcl search path.

    fcls = project.get_fcl(stage.fclname)

    # Copy the fcl file to the work directory.

    for fcl in fcls:
      workfcl = os.path.join(tmpworkdir, os.path.basename(fcl))
      if os.path.abspath(fcl) != os.path.abspath(workfcl):
        larbatch_posix.copy(fcl, workfcl)


    # Construct a wrapper fcl file (called "wrapper.fcl") that will include
    # the original fcls, plus any overrides that are dynamically generated
    # in this script.

    #print 'Making wrapper.fcl'
    wrapper_fcl_name = os.path.join(tmpworkdir, 'wrapper.fcl')
    wrapper_fcl = safeopen(wrapper_fcl_name)
    stageNum = 0
    original_project_name = project.name
    original_stage_name = stage.name
    original_project_version = project.version

    for fcl in fcls:
      wrapper_fcl.write('#---STAGE %d\n' % stageNum)
      wrapper_fcl.write('#include "%s"\n' % os.path.basename(fcl))
      wrapper_fcl.write('\n')

      # Generate overrides for sam metadata fcl parameters.
      # Only do this if our xml file appears to contain sam metadata.

      xml_has_metadata = project.file_type != '' or \
                       project.run_type != ''
      if xml_has_metadata:

        # Add overrides for FileCatalogMetadata.

        if project.release_tag != '':
            wrapper_fcl.write('services.FileCatalogMetadata.applicationVersion: "%s"\n' % \
                                  project.release_tag)
        else:
            wrapper_fcl.write('services.FileCatalogMetadata.applicationVersion: "test"\n')
        if project.file_type:
            wrapper_fcl.write('services.FileCatalogMetadata.fileType: "%s"\n' % \
                              project.file_type)
        if project.run_type:
            wrapper_fcl.write('services.FileCatalogMetadata.runType: "%s"\n' % \
                              project.run_type)


        # Add experiment-specific sam metadata.

        if stageNum < len(stage.project_name) and stage.project_name[stageNum] != '':
            project.name = stage.project_name[stageNum]
        if stageNum < len(stage.stage_name) and stage.stage_name[stageNum] != '':
            stage.name = stage.stage_name[stageNum]
        if stageNum < len(stage.project_version) and stage.project_version[stageNum] != '':
            project.version = stage.project_version[stageNum]
        sam_metadata = project_utilities.get_sam_metadata(project, stage)
        if sam_metadata:
            wrapper_fcl.write(sam_metadata)
        project.name = original_project_name
        stage.name = original_stage_name
        project.version = original_project_version

      # In case of generator jobs, add override for pubs run number
      # (subrun number is overridden inside condor_lar.sh).

      if (not stage.pubs_input and stage.pubs_output) or stage.output_run:
        wrapper_fcl.write('source.firstRun: %d\n' % stage.output_run)

      # Add overrides for genie flux parameters.
      # This section will normally be generated for any kind of generator job,
      # and should be harmless for non-genie generators.

      if stage.maxfluxfilemb != 0 and stageNum == 0:
         wrapper_fcl.write('physics.producers.generator.FluxCopyMethod: "IFDH"\n')
         wrapper_fcl.write('physics.producers.generator.MaxFluxFileMB: %d\n' % stage.maxfluxfilemb)
      wrapper_fcl.write('#---END_STAGE\n')
      stageNum = 1 + stageNum

    wrapper_fcl.close()
    #print 'Done making wrapper.fcl'

    # Get experiment setup script.  Maybe copy to work directory.
    # After this section, either variable (not both) abssetupscript or
    # setupscript will be set to a non-null value.

    abssetupscript = project_utilities.get_setup_script_path()
    setupscript = ''
    if not abssetupscript.startswith('/cvmfs/'):
        setupscript = os.path.join(stage.workdir,'setup_experiment.sh')
        larbatch_posix.copy(abssetupscript, setupscript)
        jobsub_workdir_files_args.extend(['-f', setupscript])
        abssetupscript = ''

    # Copy and rename batch script to the work directory.

    if stage.batchname != '':
        workname = stage.batchname
    else:
        workname = '%s-%s-%s' % (stage.name, project.name, project.release_tag)
    workname = workname + os.path.splitext(stage.script)[1]
    #workscript = os.path.join(tmpworkdir, workname)
    workscript = os.path.join(tmpdir, workname)
    if stage.script != workscript:
        larbatch_posix.copy(stage.script, workscript)

    # Copy and rename sam start project script to work directory.

    workstartscript = ''
    workstartname = ''
    if stage.start_script != '':
        workstartname = 'start-%s' % workname
        #workstartscript = os.path.join(tmpworkdir, workstartname)
        workstartscript = os.path.join(tmpdir, workstartname)
        if stage.start_script != workstartscript:
            larbatch_posix.copy(stage.start_script, workstartscript)

    # Copy and rename sam stop project script to work directory.

    workstopscript = ''
    workstopname = ''
    if stage.stop_script != '':
        workstopname = 'stop-%s' % workname
        #workstopscript = os.path.join(tmpworkdir, workstopname)
        workstopscript = os.path.join(tmpdir, workstopname)
        if stage.stop_script != workstopscript:
            larbatch_posix.copy(stage.stop_script, workstopscript)

    # Copy worker initialization scripts to work directory.

    for init_script in stage.init_script:
        if init_script != '':
            if not larbatch_posix.exists(init_script):
                raise RuntimeError('Worker initialization script %s does not exist.\n' % \
                    init_script)
            work_init_script = os.path.join(tmpworkdir, os.path.basename(init_script))
            if init_script != work_init_script:
                larbatch_posix.copy(init_script, work_init_script)

    # Update stage.init_script from list to single script.

    n = len(stage.init_script)
    if n == 0:
        stage.init_script = ''
    elif n == 1:
        stage.init_script = stage.init_script[0]
    else:

        # If there are multiple init scripts, generate a wrapper init script init_wrapper.sh.

        work_init_wrapper = os.path.join(tmpworkdir, 'init_wrapper.sh')
        f = open(work_init_wrapper, 'w')
        f.write('#! /bin/bash\n')
        for init_script in stage.init_script:
            f.write('echo\n')
            f.write('echo "Executing %s"\n' % os.path.basename(init_script))
            f.write('./%s\n' % os.path.basename(init_script))
            f.write('status=$?\n')
            f.write('echo "%s finished with status $status"\n' % os.path.basename(init_script))
            f.write('if [ $status -ne 0 ]; then\n')
            f.write('  exit $status\n')
            f.write('fi\n')
        f.write('echo\n')
        f.write('echo "Done executing initialization scripts."\n')
        f.close()
        stage.init_script = work_init_wrapper

    # Copy worker initialization source scripts to work directory.

    for init_source in stage.init_source:
        if init_source != '':
            if not larbatch_posix.exists(init_source):
                raise RuntimeError('Worker initialization source script %s does not exist.\n' % \
                    init_source)
        work_init_source = os.path.join(tmpworkdir, os.path.basename(init_source))
        if init_source != work_init_source:
            larbatch_posix.copy(init_source, work_init_source)

    # Update stage.init_source from list to single script.

    n = len(stage.init_source)
    if n == 0:
        stage.init_source = ''
    elif n == 1:
        stage.init_source = stage.init_source[0]
    else:

        # If there are multiple init source scripts, generate a wrapper init script
        # init_source_wrapper.sh.

        work_init_source_wrapper = os.path.join(tmpworkdir, 'init_source_wrapper.sh')
        f = open(work_init_source_wrapper, 'w')
        for init_source in stage.init_source:
            f.write('echo\n')
            f.write('echo "Sourcing %s"\n' % os.path.basename(init_source))
            f.write('source %s\n' % os.path.basename(init_source))
        f.write('echo\n')
        f.write('echo "Done sourcing initialization scripts."\n')
        f.close()
        stage.init_source = work_init_source_wrapper

    # Copy worker end-of-job scripts to work directory.

    for end_script in stage.end_script:
        if end_script != '':
            if not larbatch_posix.exists(end_script):
                raise RuntimeError('Worker end-of-job script %s does not exist.\n' % end_script)
            work_end_script = os.path.join(tmpworkdir, os.path.basename(end_script))
            if end_script != work_end_script:
                larbatch_posix.copy(end_script, work_end_script)

    # Update stage.end_script from list to single script.

    n = len(stage.end_script)
    if n == 0:
        stage.end_script = ''
    elif n == 1:
        stage.end_script = stage.end_script[0]
    else:

        # If there are multiple end scripts, generate a wrapper end script end_wrapper.sh.

        work_end_wrapper = os.path.join(tmpworkdir, 'end_wrapper.sh')
        f = open(work_end_wrapper, 'w')
        f.write('#! /bin/bash\n')
        for end_script in stage.end_script:
            f.write('echo\n')
            f.write('echo "Executing %s"\n' % os.path.basename(end_script))
            f.write('./%s\n' % os.path.basename(end_script))
            f.write('status=$?\n')
            f.write('echo "%s finished with status $status"\n' % os.path.basename(end_script))
            f.write('if [ $status -ne 0 ]; then\n')
            f.write('  exit $status\n')
            f.write('fi\n')
        f.write('echo\n')
        f.write('echo "Done executing finalization scripts."\n')
        f.close()
        stage.end_script = work_end_wrapper

    # Copy worker midstage source initialization scripts to work directory.

    for istage in stage.mid_source:
        for mid_source in stage.mid_source[istage]:
            if mid_source != '':
                if not larbatch_posix.exists(mid_source):
                    raise RuntimeError('Worker midstage initialization source script %s does not exist.\n' % mid_source)
                work_mid_source = os.path.join(tmpworkdir, os.path.basename(mid_source))
                if mid_source != work_mid_source:
                    larbatch_posix.copy(mid_source, work_mid_source)

    # Generate midstage source initialization wrapper script mid_source_wrapper.sh 
    # and update stage.mid_script to point to wrapper.
    # Note that variable $stage should be defined external to this script.

    if len(stage.mid_source) > 0:
        work_mid_source_wrapper = os.path.join(tmpworkdir, 'mid_source_wrapper.sh')
        f = open(work_mid_source_wrapper, 'w')
        for istage in stage.mid_source:
            for mid_source in stage.mid_source[istage]:
                f.write('if [ $stage -eq %d ]; then\n' % istage)
                f.write('  echo\n')
                f.write('  echo "Sourcing %s"\n' % os.path.basename(mid_source))
                f.write('  source %s\n' % os.path.basename(mid_source))
                f.write('fi\n')
        f.write('echo\n')
        f.write('echo "Done sourcing midstage source initialization scripts for stage $stage."\n')
        f.close()
        stage.mid_source = work_mid_source_wrapper
    else:
        stage.mid_source = ''

    # Copy worker midstage finalization scripts to work directory.

    for istage in stage.mid_script:
        for mid_script in stage.mid_script[istage]:
            if mid_script != '':
                if not larbatch_posix.exists(mid_script):
                    raise RuntimeError('Worker midstage finalization script %s does not exist.\n' % mid_script)
                work_mid_script = os.path.join(tmpworkdir, os.path.basename(mid_script))
                if mid_script != work_mid_script:
                    larbatch_posix.copy(mid_script, work_mid_script)

    # Generate midstage finalization wrapper script mid_wrapper.sh and update stage.mid_script 
    # to point to wrapper.

    if len(stage.mid_script) > 0:
        work_mid_wrapper = os.path.join(tmpworkdir, 'mid_wrapper.sh')
        f = open(work_mid_wrapper, 'w')
        f.write('#! /bin/bash\n')
        f.write('stage=$1\n')
        for istage in stage.mid_script:
            for mid_script in stage.mid_script[istage]:
                f.write('if [ $stage -eq %d ]; then\n' % istage)
                f.write('  echo\n')
                f.write('  echo "Executing %s"\n' % os.path.basename(mid_script))
                f.write('  ./%s\n' % os.path.basename(mid_script))
                f.write('  status=$?\n')
                f.write('  echo "%s finished with status $status"\n' % os.path.basename(mid_script))
                f.write('  if [ $status -ne 0 ]; then\n')
                f.write('    exit $status\n')
                f.write('  fi\n')
                f.write('fi\n')
        f.write('echo\n')
        f.write('echo "Done executing midstage finalization scripts for stage $stage."\n')
        f.close()
        stage.mid_script = work_mid_wrapper
    else:
        stage.mid_script = ''

    # Copy helper scripts to work directory.

    helpers = ('root_metadata.py',
               'merge_json.py',
               'subruns.py',
               'validate_in_job.py',
               'mkdir.py',
               'emptydir.py',
               'file_to_url.sh')

    for helper in helpers:

        # Find helper script in execution path.

        jobinfo = subprocess.Popen(['which', helper],
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        jobout, joberr = jobinfo.communicate()
        jobout = convert_str(jobout)
        joberr = convert_str(joberr)
        rc = jobinfo.poll()
        helper_path = jobout.splitlines()[0].strip()
        if rc == 0:
            work_helper = os.path.join(tmpworkdir, helper)
            if helper_path != work_helper:
                larbatch_posix.copy(helper_path, work_helper)
        else:
            print('Helper script %s not found.' % helper)

    # Copy helper python modules to work directory.
    # Note that for this to work, these modules must be single files.

    helper_modules = ('larbatch_posix',
                      'project_utilities',
                      'larbatch_utilities',
                      'experiment_utilities',
                      'extractor_dict')

    for helper_module in helper_modules:

        # Find helper module files.

        jobinfo = subprocess.Popen(['python'],
                                   stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        cmd = 'import %s\nprint(%s.__file__)\n' % (helper_module, helper_module)
        jobinfo.stdin.write(convert_bytes(cmd))
        jobout, joberr = jobinfo.communicate()
        jobout = convert_str(jobout)
        joberr = convert_str(joberr)
        rc = jobinfo.poll()
        helper_path = jobout.splitlines()[-1].strip()
        if rc == 0:
            #print 'helper_path = %s' % helper_path
            work_helper = os.path.join(tmpworkdir, os.path.basename(helper_path))
            if helper_path != work_helper:
                larbatch_posix.copy(helper_path, work_helper)
        else:
            print('Helper python module %s not found.' % helper_module)

    # If this is a makeup action, find list of missing files.
    # If sam information is present (cpids.list), create a makeup dataset.

    if makeup:

        checked_file = os.path.join(stage.bookdir, 'checked')
        if not larbatch_posix.exists(checked_file):
            raise RuntimeError('Wait for any running jobs to finish and run project.py --check')
        makeup_count = 0

        # First delete bad worker subdirectories.

        bad_filename = os.path.join(stage.bookdir, 'bad.list')
        if larbatch_posix.exists(bad_filename):
            lines = larbatch_posix.readlines(bad_filename)
            for line in lines:
                bad_subdir = line.strip()
                if bad_subdir != '':
                    bad_path = os.path.join(stage.outdir, bad_subdir)
                    if larbatch_posix.exists(bad_path):
                        print('Deleting %s' % bad_path)
                        larbatch_posix.rmtree(bad_path)
                    bad_path = os.path.join(stage.logdir, bad_subdir)
                    if larbatch_posix.exists(bad_path):
                        print('Deleting %s' % bad_path)
                        larbatch_posix.rmtree(bad_path)
                    bad_path = os.path.join(stage.bookdir, bad_subdir)
                    if larbatch_posix.exists(bad_path):
                        print('Deleting %s' % bad_path)
                        larbatch_posix.rmtree(bad_path)

        # Get a list of missing files, if any, for file list input.
        # Regenerate the input file list in the work directory, and
        # set the makeup job count.

        missing_files = []
        if stage.inputdef == '':
            missing_filename = os.path.join(stage.bookdir, 'missing_files.list')
            if larbatch_posix.exists(missing_filename):
                lines = larbatch_posix.readlines(missing_filename)
                for line in lines:
                    words = line.split()
                    if len(words) > 0:
                        missing_files.append(words[0])
            makeup_count = len(missing_files)
            print('Makeup list contains %d files.' % makeup_count)

        if input_list_name != '':
            work_list_name = os.path.join(tmpworkdir, input_list_name)
            if larbatch_posix.exists(work_list_name):
                larbatch_posix.remove(work_list_name)
            work_list = safeopen(work_list_name)
            for missing_file in missing_files:
                work_list.write('%s\n' % missing_file)
            work_list.close()

        # In case of making up generation jobs, produce a procmap file
        # for missing jobs that will ensure that made up generation
        # jobs get a unique subrun.

        if stage.inputdef == '' and stage.inputfile == '' and stage.inputlist == '':
            procs = set(range(stage.num_jobs))

            # Loop over good output files to extract existing
            # process numbers and determine missing process numbers.

            output_files = os.path.join(stage.bookdir, 'files.list')
            if larbatch_posix.exists(output_files):
                lines = larbatch_posix.readlines(output_files)
                for line in lines:
                    dir = os.path.basename(os.path.dirname(line))
                    dir_parts = dir.split('_')
                    if len(dir_parts) > 1:
                        proc = int(dir_parts[1])
                        if proc in procs:
                            procs.remove(proc)
                if len(procs) != makeup_count:
                    raise RuntimeError('Makeup process list has different length than makeup count.')

                # Generate process map.

                if len(procs) > 0:
                    procmap = 'procmap.txt'
                    procmap_path = os.path.join(tmpworkdir, procmap)
                    procmap_file = safeopen(procmap_path)
                    for proc in procs:
                        procmap_file.write('%d\n' % proc)
                    procmap_file.close()

        # Prepare sam-related makeup information.

        import_samweb()

        # Get list of successful consumer process ids.

        cpids = []
        cpids_filename = os.path.join(stage.bookdir, 'cpids.list')
        if larbatch_posix.exists(cpids_filename):
            cpids_files = larbatch_posix.readlines(cpids_filename)
            for line in cpids_files:
                cpids.append(line.strip())

        # Create makeup dataset definition.

        makeup_defname = ''
        if len(cpids) > 0:
            project_utilities.test_kca()
            makeup_defname = samweb.makeProjectName(stage.inputdef) + '_makeup'

            # Construct comma-separated list of consumer process ids.

            cpids_list = ''
            sep = ''
            for cpid in cpids:
                cpids_list = cpids_list + '%s%s' % (sep, cpid)
                sep = ','

            # Construct makeup dimension.

            dim = '(defname: %s) minus (consumer_process_id %s and consumed_status consumed)' % (stage.inputdef, cpids_list)

            # Create makeup dataset definition.

            print('Creating makeup sam dataset definition %s' % makeup_defname)
            project_utilities.test_kca()
            samweb.createDefinition(defname=makeup_defname, dims=dim)
            makeup_count = samweb.countFiles(defname=makeup_defname)
            print('Makeup dataset contains %d files.' % makeup_count)

    # Make a tarball out of all of the files in tmpworkdir in stage.workdir

    tmptar = '%s/work.tar' % tmpworkdir
    jobinfo = subprocess.Popen(['tar','-cf', tmptar, '-C', tmpworkdir,
                                '--mtime=2018-01-01',
                                '--exclude=work.tar', '.'],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    jobout, joberr = jobinfo.communicate()
    rc = jobinfo.poll()
    if rc != 0:
        raise RuntimeError('Failed to create work tarball in %s' % tmpworkdir)

    # Calculate the checksum of the tarball.

    hasher = hashlib.md5()
    f = open(tmptar, 'rb')
    buf = f.read(1024)
    while len(buf) > 0:
        hasher.update(buf)
        buf = f.read(1024)
    hash = hasher.hexdigest()
    f.close()

    # Transfer tarball to work directory.
    # Give the tarball a unique name based on its checksum.
    # Don't replace the tarball if it already exists.

    hashtar = '%s/work%s.tar' % (stage.workdir, hash)
    if not larbatch_posix.exists(hashtar):
        larbatch_posix.copy(tmptar, hashtar)
    jobsub_workdir_files_args.extend(['-f', hashtar])

    # Sam stuff.

    # Get input sam dataset definition name.
    # Can be from xml or a makeup dataset that we just created.

    inputdef = stage.inputdef
    if makeup and makeup_defname != '':
        inputdef = makeup_defname

    # Sam project name.

    prjname = ''
    if inputdef != '':
        import_samweb()
        project_utilities.test_kca()
        prjname = samweb.makeProjectName(inputdef)

    # Get mix input sam dataset definition name.

    mixprjname = ''
    if stage.mixinputdef != '':
        import_samweb()
        project_utilities.test_kca()
        mixprjname = 'mix_%s' % samweb.makeProjectName(stage.mixinputdef)

    # If the prestart flag is specified, start the sam project now.

    prj_started = False
    if prjname != '' and stage.prestart != 0:
        ok = project_utilities.start_project(inputdef, prjname,
                                             stage.num_jobs * stage.max_files_per_job,
                                             stage.recur, stage.filelistdef)
        if ok != 0:
            print('Failed to start project.')
            sys.exit(1)
        prj_started = True

    # Also start mix project, if any.

    if mixprjname != '' and prj_started:
        ok = project_utilities.start_project(stage.mixinputdef, mixprjname, 0, 0, stage.filelistdef)
        if ok != 0:
            print('Failed to start mix project.')
            sys.exit(1)

    # Get role

    role = project_utilities.get_role()
    if project.role != '':
        role = project.role

    # Construct jobsub command line for workers.

    command = ['jobsub_submit']
    command_njobs = 1

    # Jobsub options.

    command.append('--group=%s' % project_utilities.get_experiment())
    command.append('--role=%s' % role)
    command.extend(jobsub_workdir_files_args)
    if project.server != '-' and project.server != '':
        command.append('--jobsub-server=%s' % project.server)
    if stage.resource != '':
        command.append('--resource-provides=usage_model=%s' % stage.resource)
    elif project.resource != '':
        command.append('--resource-provides=usage_model=%s' % project.resource)
    if stage.lines != '':
        command.append('--lines=%s' % stage.lines)
    elif project.lines != '':
        command.append('--lines=%s' % project.lines)
    if stage.site != '':
        command.append('--site=%s' % stage.site)
    if stage.blacklist != '':
        command.append('--blacklist=%s' % stage.blacklist)
    if stage.cpu != 0:
        command.append('--cpu=%d' % stage.cpu)
    if stage.disk != '':
        command.append('--disk=%s' % stage.disk)
    if stage.memory != 0:
        command.append('--memory=%d' % stage.memory)
    if project.os != '':
        if stage.singularity == 0:
            command.append('--OS=%s' % project.os)
        else:
            p = project_utilities.get_singularity(project.os)
            if p != '':
                if (stage.num_jobs > 1 or project.force_dag) and \
                   (inputdef != '' or stage.mixinputdef != '') :
                    command.append(r"""--lines='+SingularityImage=\"%s\"'""" % p)
                else:
                    command.append(r"""--lines='+SingularityImage="%s"'""" % p)
            else:
                raise RuntimeError('No singularity image found for %s' % project.os)
    if not stage.pubs_output:
        if not makeup:
            command_njobs = stage.num_jobs
            command.extend(['-N', '%d' % command_njobs])
        else:
            command_njobs = min(makeup_count, stage.num_jobs)
            command.extend(['-N', '%d' % command_njobs])
    else:
        if stage.inputdef != '':
            command_njobs = stage.num_jobs
        else:
            command_njobs = stage.num_jobs
            command.extend(['-N', '%d' % command_njobs])
    if stage.jobsub != '':
        for word in stage.jobsub.split():
            command.append(word)
    opt = project_utilities.default_jobsub_submit_options()
    if opt != '':
        for word in opt.split():
            command.append(word)
    if stage.cvmfs != 0:
        command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_opensciencegrid_org==true)\'' % project_utilities.get_experiment())
    if stage.stash != 0:
        command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_osgstorage_org==true)\'' % project_utilities.get_experiment())
    if stage.singularity != 0:
        command.append('--append_condor_requirements=\'(TARGET.HAS_SINGULARITY=?=true)\'')

    # Batch script.

    workurl = "file://%s" % workscript
    command.append(workurl)

    # check if there is a request for max num of files per job
    # and add that if to the condor_lar.sh line

    if stage.max_files_per_job != 0:
        command_max_files_per_job = stage.max_files_per_job
        command.extend(['--nfile', '%d' % command_max_files_per_job])
        #print 'Setting the max files to %d' % command_max_files_per_job

    # Larsoft options.

    command.extend([' --group', project_utilities.get_experiment()])
    command.extend([' -g'])
    command.extend([' -c', 'wrapper.fcl'])
    command.extend([' --ups', ','.join(project.ups)])
    if project.release_tag != '':
        command.extend([' -r', project.release_tag])
    command.extend([' -b', project.release_qual])
    if project.local_release_dir != '':
        command.extend([' --localdir', project.local_release_dir])
    if project.local_release_tar != '':
        command.extend([' --localtar', project.local_release_tar])
    command.extend([' --workdir', stage.workdir])
    command.extend([' --outdir', stage.outdir])
    command.extend([' --logdir', stage.logdir])
    if stage.dirsize > 0:
        command.extend([' --dirsize', '%d' % stage.dirsize])
    if stage.dirlevels > 0:
        command.extend([' --dirlevels', '%d' % stage.dirlevels])
    if stage.exe:
        if type(stage.exe) == type([]):
            command.extend([' --exe', ':'.join(stage.exe)])
        else:
            command.extend([' --exe', stage.exe])
    if stage.schema != '':
        command.extend([' --sam_schema', stage.schema])
    if project.os != '':
        command.extend([' --os', project.os])

    # Set the process number for pubs jobs that are the first in the chain.

    if not stage.pubs_input and stage.pubs_output and stage.output_subruns[0] > 0:
        command.extend(['--process', '%d' % (stage.output_subruns[0]-1)])

    # Specify single worker mode in case of pubs output.

    if stage.dynamic:
        command.append('--single')

    if stage.inputfile != '':
        command.extend([' -s', stage.inputfile])
    elif input_list_name != '':
        command.extend([' -S', input_list_name])
    elif inputdef != '':
        command.extend([' --sam_defname', inputdef,
                        ' --sam_project', prjname])
    if recur:
        command.extend([' --recur'])
    if stage.mixinputdef != '':
        command.extend([' --mix_defname', stage.mixinputdef,
                        ' --mix_project', mixprjname])
    if stage.inputmode != '':
        command.extend([' --inputmode', stage.inputmode])
    command.extend([' -n', '%d' % stage.num_events])
    if stage.inputdef == '':
        command.extend([' --njobs', '%d' % stage.num_jobs ])
    for ftype in stage.datafiletypes:
        command.extend(['--data_file_type', ftype])
    if procmap != '':
        command.extend([' --procmap', procmap])
    if stage.output:
        if type(stage.output) == type([]):
            command.extend([' --output', ':'.join(stage.output)])
        else:
            command.extend([' --output', stage.output])
    if stage.TFileName != '':
        command.extend([' --TFileName', stage.TFileName])
    if stage.init_script != '':
        command.extend([' --init-script', os.path.basename(stage.init_script)])
    if stage.init_source != '':
        command.extend([' --init-source', os.path.basename(stage.init_source)])
    if stage.end_script != '':
        command.extend([' --end-script', os.path.basename(stage.end_script)])
    if stage.mid_source != '':
        command.extend([' --mid-source', os.path.basename(stage.mid_source)])
    if stage.mid_script != '':
        command.extend([' --mid-script', os.path.basename(stage.mid_script)])
    if abssetupscript != '':
        command.extend([' --init', abssetupscript])


    #print 'Will Validation will be done on the worker node %d' % stage.validate_on_worker
    if stage.validate_on_worker == 1:
      print('Validation will be done on the worker node %d' % stage.validate_on_worker)
      command.extend([' --validate'])
      command.extend([' --declare'])
      # Maintain parentage only if we have multiple fcl files and thus are running in multiple stages
      if type(stage.fclname) == type([]) and len(stage.fclname) > 1:
        command.extend([' --maintain_parentage'])

    if stage.copy_to_fts == 1:
      command.extend([' --copy'])

    # If input is from sam, also construct a dag file, or add --sam_start option.

    if (prjname != '' or mixprjname != '') and command_njobs == 1 and not project.force_dag and not prj_started:
        command.extend([' --sam_start',
                        ' --sam_station', project_utilities.get_experiment(),
                        ' --sam_group', project_utilities.get_experiment()])


    # At this point, the main batch worker command is complete.
    # Decide whether to submit this command stand alone or as part of a dag.

    start_commands = []
    stop_commands = []
    dag_prjs = []
    if command_njobs > 1 or project.force_dag:
        if inputdef != '':
            dag_prjs.append([inputdef, prjname])
        if stage.mixinputdef != '':
            dag_prjs.append([stage.mixinputdef, mixprjname])

    for dag_prj in dag_prjs:

        # At this point, it is an error if the start and stop project
        # scripts were not found.

        if workstartname == '' or workstopname == '':
            raise RuntimeError('Sam start or stop project script not found.')

        # Start project jobsub command.

        start_command = ['jobsub']

        # General options.

        start_command.append('--group=%s' % project_utilities.get_experiment())
        if setupscript != '':
            start_command.append('-f %s' % setupscript)
        #start_command.append('--role=%s' % role)
        if stage.resource != '':
            start_command.append('--resource-provides=usage_model=%s' % stage.resource)
        elif project.resource != '':
            start_command.append('--resource-provides=usage_model=%s' % project.resource)
        if stage.lines != '':
            start_command.append('--lines=%s' % stage.lines)
        elif project.lines != '':
            start_command.append('--lines=%s' % project.lines)
        if stage.site != '':
            start_command.append('--site=%s' % stage.site)
        if stage.blacklist != '':
            start_command.append('--blacklist=%s' % stage.blacklist)
        if project.os != '':
            if stage.singularity == 0:
                start_command.append('--OS=%s' % project.os)
            else:
                p = project_utilities.get_singularity(project.os)
                if p != '':
                    start_command.append('--lines=\'+SingularityImage=\\"%s\\"\'' % p)
                else:
                    raise RuntimeError('No singularity image found for %s' % project.os)
        if stage.jobsub_start != '':
            for word in stage.jobsub_start.split():
                start_command.append(word)
        opt = project_utilities.default_jobsub_submit_options()
        if opt != '':
            for word in opt.split():
                start_command.append(word)
        if stage.cvmfs != 0:
            start_command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_opensciencegrid_org==true)\'' % project_utilities.get_experiment())
        if stage.stash != 0:
            start_command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_osgstorage_org==true)\'' % project_utilities.get_experiment())
        if stage.singularity != 0:
            start_command.append('--append_condor_requirements=\'(TARGET.HAS_SINGULARITY=?=true)\'')

        # Start project script.

        workstarturl = "file://%s" % workstartscript
        start_command.append(workstarturl)

        # Sam options.

        start_command.extend([' --sam_station', project_utilities.get_experiment(),
                              ' --sam_group', project_utilities.get_experiment(),
                              ' --sam_defname', dag_prj[0],
                              ' --sam_project', dag_prj[1],
                              ' -g'])
        if recur:
            start_command.extend([' --recur'])

        if abssetupscript != '':
            start_command.extend([' --init', abssetupscript])

        if stage.num_jobs > 0 and stage.max_files_per_job > 0:
            start_command.extend([' --max_files', '%d' % (stage.num_jobs * stage.max_files_per_job)])

        if stage.prestagefraction > 0.:
            start_command.extend([' --prestage_fraction', '%f' % stage.prestagefraction])

        # Output directory.

        start_command.extend([' --logdir', stage.logdir])

        # Done with start command.

        if not prj_started or stage.prestagefraction > 0.:
            start_commands.append(start_command)

        # Stop project jobsub command.

        stop_command = ['jobsub']

        # General options.

        stop_command.append('--group=%s' % project_utilities.get_experiment())
        if setupscript != '':
            stop_command.append('-f %s' % setupscript)
        #stop_command.append('--role=%s' % role)
        if stage.resource != '':
            stop_command.append('--resource-provides=usage_model=%s' % stage.resource)
        elif project.resource != '':
            stop_command.append('--resource-provides=usage_model=%s' % project.resource)
        if stage.lines != '':
            stop_command.append('--lines=%s' % stage.lines)
        elif project.lines != '':
            stop_command.append('--lines=%s' % project.lines)
        if stage.site != '':
            stop_command.append('--site=%s' % stage.site)
        if stage.blacklist != '':
            stop_command.append('--blacklist=%s' % stage.blacklist)
        if project.os != '':
            if stage.singularity == 0:
                stop_command.append('--OS=%s' % project.os)
            else:
                p = project_utilities.get_singularity(project.os)
                if p != '':
                    stop_command.append('--lines=\'+SingularityImage=\\"%s\\"\'' % p)
                else:
                    raise RuntimeError('No singularity image found for %s' % project.os)
        if stage.jobsub_start != '':
            for word in stage.jobsub_start.split():
                stop_command.append(word)
        opt = project_utilities.default_jobsub_submit_options()
        if opt != '':
            for word in opt.split():
                stop_command.append(word)
        if stage.cvmfs != 0:
            stop_command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_opensciencegrid_org==true)\'' % project_utilities.get_experiment())
        if stage.stash != 0:
            stop_command.append('--append_condor_requirements=\'(TARGET.HAS_CVMFS_%s_osgstorage_org==true)\'' % project_utilities.get_experiment())
        if stage.singularity != 0:
            stop_command.append('--append_condor_requirements=\'(TARGET.HAS_SINGULARITY=?=true)\'')

        # Stop project script.

        workstopurl = "file://%s" % workstopscript
        stop_command.append(workstopurl)

        # Sam options.

        stop_command.extend([' --sam_station', project_utilities.get_experiment(),
                             ' --sam_project', dag_prj[1],
                             ' -g'])

        # Output directory.

        stop_command.extend([' --logdir', stage.logdir])

        if abssetupscript != '':
            stop_command.extend([' --init', abssetupscript])

        # Done with stop command.

        stop_commands.append(stop_command)

    if len(start_commands) > 0 or len(stop_commands) > 0:

        # Create dagNabbit.py configuration script in the work directory.

        dagfilepath = os.path.join(tmpdir, 'submit.dag')
        dag = safeopen(dagfilepath)
        dag.write('<serial>\n')

        # Write start section.

        if len(start_commands) > 0:
            dag.write('\n<parallel>\n\n')
            for start_command in start_commands:
                first = True
                for word in start_command:
                    if not first:
                        dag.write(' ')
                    dag.write(word)
                    if word[:6] == 'jobsub':
                        dag.write(' -n')
                    first = False
                dag.write('\n\n')
            dag.write('</parallel>\n')

        # Write main section.

        dag.write('\n<parallel>\n\n')
        for process in range(command_njobs):
        #for process in range(1):
            first = True
            skip = False
            for word in command:
                if skip:
                    skip = False
                else:
                    if word == '-N':
                    #if False:
                        skip = True
                    else:
                        if not first:
                            dag.write(' ')
                        if word[:6] == 'jobsub':
                            word = 'jobsub'
                        if word[:7] == '--role=':
                            word = ''
                        if word.startswith('--jobsub-server='):
                            word = ''
                        word = project_utilities.dollar_escape(word)
                        dag.write(word)
                        if word[:6] == 'jobsub':
                            dag.write(' -n')
                        first = False
            dag.write(' --process %d\n' % process)
            dag.write('\n')
        dag.write('\n</parallel>\n')

        # Write stop section.

        if len(stop_commands) > 0:
            dag.write('\n<parallel>\n\n')
            for stop_command in stop_commands:
                first = True
                for word in stop_command:
                    if not first:
                        dag.write(' ')
                    dag.write(word)
                    if word[:6] == 'jobsub':
                        dag.write(' -n')
                    first = False
                dag.write('\n\n')
            dag.write('</parallel>\n')

        # Finish dag.

        dag.write('\n</serial>\n')
        dag.close()

        # Update the main submission command to use jobsub_submit_dag instead of jobsub_submit.

        command = ['jobsub_submit_dag']
        command.append('--group=%s' % project_utilities.get_experiment())
        if project.server != '-' and project.server != '':
            command.append('--jobsub-server=%s' % project.server)
        command.append('--role=%s' % role)
        dagfileurl = 'file://'+ dagfilepath
        command.append(dagfileurl)

    checked_file = os.path.join(stage.bookdir, 'checked')

    # Calculate submit timeout.

    submit_timeout = 3600000
    if prjname != '':
        submit_timeout += 1.0 * command_njobs
    if stage.jobsub_timeout > submit_timeout:
        submit_timeout = stage.jobsub_timeout

    # Submit jobs.

    if not makeup:

        # For submit action, invoke the job submission command.

        print('Invoke jobsub_submit')
        if dryrun:
            print(' '.join(command))
        else:
            q = queue.Queue()
            jobinfo = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            thread = threading.Thread(target=project_utilities.wait_for_subprocess, args=[jobinfo, q])
            thread.start()
            thread.join(timeout=submit_timeout)
            if thread.is_alive():
                jobinfo.terminate()
                thread.join()
            rc = q.get()
            jobout = convert_str(q.get())
            joberr = convert_str(q.get())
            if larbatch_posix.exists(checked_file):
                larbatch_posix.remove(checked_file)
            if larbatch_posix.isdir(tmpdir):
                larbatch_posix.rmtree(tmpdir)
            if larbatch_posix.isdir(tmpworkdir):
                larbatch_posix.rmtree(tmpworkdir)
            if rc != 0:
                raise JobsubError(command, rc, jobout, joberr)
            for line in jobout.split('\n'):
                if "JobsubJobId" in line:
                    jobid = line.strip().split()[-1]
            if not jobid:
                raise JobsubError(command, rc, jobout, joberr)
        print('jobsub_submit finished.')

    else:

        # For makeup action, abort if makeup job count is zero for some reason.

        if makeup_count > 0:
            if dryrun:
                print(' '.join(command))
            else:
                q = queue.Queue()
                jobinfo = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                thread = threading.Thread(target=project_utilities.wait_for_subprocess,
                                          args=[jobinfo, q])
                thread.start()
                thread.join(timeout=submit_timeout)
                if thread.is_alive():
                    jobinfo.terminate()
                    thread.join()
                rc = q.get()
                jobout = convert_str(q.get())
                joberr = convert_str(q.get())
                if larbatch_posix.exists(checked_file):
                    larbatch_posix.remove(checked_file)
                if larbatch_posix.isdir(tmpdir):
                    larbatch_posix.rmtree(tmpdir)
                if larbatch_posix.isdir(tmpworkdir):
                    larbatch_posix.rmtree(tmpworkdir)
                if rc != 0:
                    raise JobsubError(command, rc, jobout, joberr)
                for line in jobout.split('\n'):
                    if "JobsubJobId" in line:
                        jobid = line.strip().split()[-1]
                if not jobid:
                    raise JobsubError(command, rc, jobout, joberr)
        else:
            print('Makeup action aborted because makeup job count is zero.')

    # Done.

    return jobid


# Submit/makeup action.

def dosubmit(project, stage, makeup=False, recur=False, dryrun=False):

    # Make sure we have a kerberos ticket.

    project_utilities.test_kca()

    # Make sure jobsub_client is available.

    larbatch_utilities.test_jobsub()

    # Run presubmission check script.

    ok = stage.checksubmit()
    if ok != 0:
        print('No jobs submitted.')
        return

    # In pubs mode, delete any existing work, log, or output
    # directories, since there is no separate makeup action for pubs
    # mode.

    if stage.pubs_output and not stage.dynamic:
        if larbatch_posix.exists(stage.workdir):
            larbatch_posix.rmtree(stage.workdir)
        if larbatch_posix.exists(stage.outdir):
            larbatch_posix.rmtree(stage.outdir)
        if larbatch_posix.exists(stage.logdir):
            larbatch_posix.rmtree(stage.logdir)
        if larbatch_posix.exists(stage.bookdir):
            larbatch_posix.rmtree(stage.bookdir)

    # Make or check directories.

    if not makeup:
        stage.makedirs()
    else:
        stage.checkdirs()

    # Check input files.

    ok = stage.checkinput(checkdef=True)
    if ok != 0:
        print('No jobs submitted.')
        return

    # Make sure output and log directories are empty (submit only).

    if not makeup and not recur and not stage.dynamic:
        if len(larbatch_posix.listdir(stage.outdir)) != 0:
            raise RuntimeError('Output directory %s is not empty.' % stage.outdir)
        if len(larbatch_posix.listdir(stage.logdir)) != 0:
            raise RuntimeError('Log directory %s is not empty.' % stage.logdir)
        if len(larbatch_posix.listdir(stage.bookdir)) != 0:
            raise RuntimeError('Log directory %s is not empty.' % stage.bookdir)

    # Copy files to workdir and issue jobsub command to submit jobs.

    jobid = dojobsub(project, stage, makeup, recur, dryrun)

    # Append jobid to file "jobids.list" in the log directory.

    jobids_filename = os.path.join(stage.bookdir, 'jobids.list')
    jobids = []
    if larbatch_posix.exists(jobids_filename):
        lines = larbatch_posix.readlines(jobids_filename)
        for line in lines:
            id = line.strip()
            if len(id) > 0:
                jobids.append(id)
    if len(jobid) > 0:
        jobids.append(jobid)

    jobid_file = safeopen(jobids_filename)
    for jobid in jobids:
        jobid_file.write('%s\n' % jobid)
    jobid_file.close()

    # Done.

    return jobid

# Merge histogram files.
# If mergehist is True, merge histograms using "hadd -T".
# If mergentuple is True, do full merge using "hadd".
# If neither argument is True, do custom merge using merge program specified
# in xml stage.

def domerge(stage, mergehist, mergentuple):

    hlist = []
    hnlist = os.path.join(stage.bookdir, 'filesana.list')
    if larbatch_posix.exists(hnlist):
        hlist = larbatch_posix.readlines(hnlist)
    else:
        raise RuntimeError('No filesana.list file found %s, run project.py --checkana' % hnlist)

    histurlsname_temp = 'histurls.list'
    histurls = safeopen(histurlsname_temp)

    for hist in hlist:
        histurls.write('%s\n' % hist)
    histurls.close()

    if len(hlist) > 0:
        name = os.path.join(stage.outdir, 'anahist.root')
        if name[0:6] == '/pnfs/':
            tempdir = '%s/mergentuple_%d_%d' % (project_utilities.get_scratch_dir(),
                                                os.getuid(),
                                                os.getpid())
            if not larbatch_posix.isdir(tempdir):
                larbatch_posix.makedirs(tempdir)
            name_temp = '%s/anahist.root' % tempdir
        else:
            name_temp = name

        if mergehist:
            mergecom = "hadd -T"
        elif mergentuple:
            mergecom = "hadd"
        else:
            mergecom = stage.merge

        print("Merging %d root files using %s." % (len(hlist), mergecom))

        if larbatch_posix.exists(name_temp):
            larbatch_posix.remove(name_temp)
        comlist = mergecom.split()
        comlist.extend(["-f", "-k", name_temp, '@' + histurlsname_temp])
        rc = subprocess.call(comlist, stdout=sys.stdout, stderr=sys.stderr)
        if rc != 0:
            print("%s exit status %d" % (mergecom, rc))
        if name != name_temp:
            if larbatch_posix.exists(name):
                larbatch_posix.remove(name)
            if larbatch_posix.exists(name_temp):

                # Copy merged file.
                larbatch_posix.copy(name_temp, name)
                larbatch_posix.rmtree(tempdir)
        larbatch_posix.remove(histurlsname_temp)


# Sam audit.

def doaudit(stage):

    import_samweb()
    stage_has_input = stage.inputfile != '' or stage.inputlist != '' or stage.inputdef != ''
    if not stage_has_input:
        raise RuntimeError('No auditing for generator stage.')

    # Are there other ways to get output files other than through definition!?

    outputlist = []
    outparentlist = []
    if stage.defname != '':
        query = 'isparentof: (defname: %s) and availability: anylocation' %(stage.defname)
        try:
            outparentlist = samweb.listFiles(dimensions=query)
            outputlist = samweb.listFiles(defname=stage.defname)
        except:
            raise RuntimeError('Error accessing sam information for definition %s.\nDoes definition exist?' % stage.defname)
    else:
        raise RuntimeError('Output definition not found.')

    # To get input files one can use definition or get inputlist given to that stage or
    # get input files for a given stage as get_input_files(stage)

    inputlist = []
    if stage.inputdef != '':
        import_samweb()
        inputlist=samweb.listFiles(defname=stage.inputdef)
    elif stage.inputlist != '':
        ilist = []
        if larbatch_posix.exists(stage.inputlist):
            ilist = larbatch_posix.readlines(stage.inputlist)
            inputlist = []
            for i in ilist:
                inputlist.append(os.path.basename(i.strip()))
    else:
        raise RuntimeError('Input definition and/or input list does not exist.')

    difflist = set(inputlist)^set(outparentlist)
    mc = 0;
    me = 0;
    for item in difflist:
        if item in inputlist:
            mc = mc+1
            if mc==1:
                missingfilelistname = os.path.join(stage.bookdir, 'missingfiles.list')
                missingfilelist = safeopen(missingfilelistname)
                if mc>=1:
                    missingfilelist.write("%s\n" %item)
        elif item in outparentlist:
            me = me+1
            childcmd = 'samweb list-files "ischildof: (file_name=%s) and availability: physical"' %(item)
            children = convert_str(subprocess.check_output(childcmd, shell=True)).splitlines()
            rmfile = list(set(children) & set(outputlist))[0]
            if me ==1:
                flist = []
                fnlist = os.path.join(stage.bookdir, 'files.list')
                if larbatch_posix.exists(fnlist):
                    flist = larbatch_posix.readlines(fnlist)
                    slist = []
                    for line in flist:
                        slist.append(line.split()[0])
                else:
                    raise RuntimeError('No files.list file found %s, run project.py --check' % fnlist)

            # Declare the content status of the file as bad in SAM.

            sdict = {'content_status':'bad'}
            project_utilities.test_kca()
            samweb.modifyFileMetadata(rmfile, sdict)
            print('\nDeclaring the status of the following file as bad:', rmfile)

            # Remove this file from the files.list in the output directory.

            fn = []
            fn = [x for x in slist if os.path.basename(x.strip()) != rmfile]
            thefile = safeopen(fnlist)
            for item in fn:
                thefile.write("%s\n" % item)

    if mc==0 and me==0:
        print("Everything in order.")
        return 0
    else:
        print('Missing parent file(s) = ', mc)
        print('Extra parent file(s) = ',me)

    if mc != 0:
        missingfilelist.close()
        print("Creating missingfiles.list in the output directory....done!")
    if me != 0:
        thefile.close()
        #larbatch_posix.remove("jsonfile.json")
        print("For extra parent files, files.list redefined and content status declared as bad in SAM...done!")


# Print help.

def help():

    filename = sys.argv[0]
    file = open(filename, 'r')

    doprint=0

    for line in file.readlines():
        if line[2:12] == 'project.py':
            doprint = 1
        elif line[0:6] == '######' and doprint:
            doprint = 0
        if doprint:
            if len(line) > 2:
                print(line[2:], end=' ')
            else:
                print()

# Normalize xml path.
#
# Don't modify xml file path for any of the following cases.
#
# 1.  xmlfile contains character ':'.  In this case xmlfile may be a url.
# 2.  xmlfile starts with '/', './' or '../'.
# 3.  xmlfile is '-'.  Stands for standard input.
#
# Otherwise, assume that xmlfile is a relative path.  In this case, convert it to
# an absolute path relative to the current working directory, or directory contained
# in environment variable XMLPATH (colon-separated list of directories).

def normxmlpath(xmlfile):

    # Default result = input.

    normxmlfile = xmlfile

    # Does this look like a relative path?

    if xmlfile.find(':') < 0 and \
       not xmlfile.startswith('/') and \
       not xmlfile.startswith('./') and \
       not xmlfile.startswith('../') and \
       xmlfile != '-':

        # Yes, try to normalize path.
        # Construct a list of directories to search, starting with current working directory.

        dirs = [os.getcwd()]

        # Add directories in environment variable XMLPATH, if defined.

        if 'XMLPATH' in os.environ:
            dirs.extend(os.environ['XMLPATH'].split(':'))

        # Loop over directories.

        for dir in dirs:
            xmlpath = os.path.join(dir, xmlfile)
            if os.path.exists(xmlpath):
                normxmlfile = xmlpath
                break

    # Done.

    return normxmlfile

# Print xml help.

def xmlhelp():

    filename = sys.argv[0]
    file = open(filename, 'r')

    doprint=0

    for line in file.readlines():
        if line[2:20] == 'XML file structure':
            doprint = 1
        elif line[0:6] == '######' and doprint:
            doprint = 0
        if doprint:
            if len(line) > 2:
                print(line[2:], end=' ')
            else:
                print()


# Main program.

def main(argv):

    # Parse arguments.

    xmlfile = ''
    projectname = ''
    stagenames = ['']
    lines = ''
    site = ''
    cpu = 0
    disk = ''
    memory = 0
    inputdef = ''
    merge = 0
    submit = 0
    recur = 0
    pubs = 0
    pubs_run = 0
    pubs_subruns = []
    pubs_version = None
    check = 0
    checkana = 0
    shorten = 0
    fetchlog = 0
    mergehist = 0
    mergentuple = 0
    audit = 0
    stage_status = 0
    makeup = 0
    clean = 0
    clean_one = 0
    dump_project = 0
    dump_stage = 0
    dryrun = 0
    nocheck = 0
    print_outdir = 0
    print_logdir = 0
    print_workdir = 0
    print_bookdir = 0
    fcl = 0
    defname = 0
    do_input_files = 0
    do_check_submit = 0
    do_check_input = 0
    declare = 0
    declare_ana = 0
    define = 0
    define_ana = 0
    undefine = 0
    check_declarations = 0
    check_declarations_ana = 0
    test_declarations = 0
    test_declarations_ana = 0
    check_definition = 0
    check_definition_ana = 0
    test_definition = 0
    test_definition_ana = 0
    add_locations = 0
    add_locations_ana = 0
    check_locations = 0
    check_locations_ana = 0
    upload = 0
    upload_ana = 0
    check_tape = 0
    check_tape_ana = 0
    clean_locations = 0
    clean_locations_ana = 0
    remove_locations = 0
    remove_locations_ana = 0

    args = argv[1:]
    while len(args) > 0:
        if args[0] == '-h' or args[0] == '--help' :
            help()
            return 0
        elif args[0] == '-xh' or args[0] == '--xmlhelp' :
            xmlhelp()
            return 0
        elif args[0] == '--xml' and len(args) > 1:
            xmlfile = args[1]
            del args[0:2]
        elif args[0] == '--project' and len(args) > 1:
            projectname = args[1]
            del args[0:2]
        elif args[0] == '--stage' and len(args) > 1:
            stagenames = args[1].split(',')
            del args[0:2]
        elif args[0] == '--tmpdir' and len(args) > 1:
            os.environ['TMPDIR'] = args[1]
            del args[0:2]
        elif args[0] == '--lines' and len(args) > 1:
            lines = args[1]
            del args[0:2]
        elif args[0] == '--site' and len(args) > 1:
            site = args[1]
            del args[0:2]
        elif args[0] == '--cpu' and len(args) > 1:
            cpu = int(args[1])
            del args[0:2]
        elif args[0] == '--disk' and len(args) > 1:
            disk = args[1]
            del args[0:2]
        elif args[0] == '--memory' and len(args) > 1:
            memory = int(args[1])
            del args[0:2]
        elif args[0] == '--inputdef' and len(args) > 1:
            inputdef = args[1]
            del args[0:2]
        elif args[0] == '--submit':
            submit = 1
            del args[0]
        elif args[0] == '--recur':
            recur = 1
            del args[0]
        elif args[0] == '--pubs' and len(args) > 2:
            pubs = 1
            pubs_run = int(args[1])
            pubs_subruns = project_utilities.parseInt(args[2])
            del args[0:3]
            if len(args) > 0 and args[0] != '' and args[0][0] != '-':
                pubs_version = int(args[0])
                del args[0]
        elif args[0] == '--check':
            check = 1
            del args[0]
        elif args[0] == '--checkana':
            checkana = 1
            del args[0]
        elif args[0] == '--shorten':
            shorten = 1
            del args[0]
        elif args[0] == '--fetchlog':
            fetchlog = 1
            del args[0]
        elif args[0] == '--merge':
            merge = 1
            del args[0]
        elif args[0] == '--mergehist':
            mergehist = 1
            del args[0]
        elif args[0] == '--mergentuple':
            mergentuple = 1
            del args[0]
        elif args[0] == '--audit':
            audit = 1
            del args[0]
        elif args[0] == '--status':
            stage_status = 1
            del args[0]
        elif args[0] == '--makeup':
            makeup = 1
            del args[0]
        elif args[0] == '--clean':
            clean = 1
            del args[0]
        elif args[0] == '--clean_one':
            clean_one = 1
            del args[0]
        elif args[0] == '--dump_project':
            dump_project = 1
            del args[0]
        elif args[0] == '--dump_stage':
            dump_stage = 1
            del args[0]
        elif args[0] == '--dryrun':
            dryrun = 1
            del args[0]
        elif args[0] == '--nocheck':
            nocheck = 1
            del args[0]
        elif args[0] == '--outdir':
            print_outdir = 1
            del args[0]
        elif args[0] == '--logdir':
            print_logdir = 1
            del args[0]
        elif args[0] == '--workdir':
            print_workdir = 1
            del args[0]
        elif args[0] == '--bookdir':
            print_bookdir = 1
            del args[0]
        elif args[0] == '--fcl':
            fcl = 1
            del args[0]
        elif args[0] == '--defname':
            defname = 1
            del args[0]
        elif args[0] == '--input_files':
            do_input_files = 1
            del args[0]
        elif args[0] == '--check_submit':
            do_check_submit = 1
            del args[0]
        elif args[0] == '--check_input':
            do_check_input = 1
            del args[0]
        elif args[0] == '--declare':
            declare = 1
            del args[0]
        elif args[0] == '--declare_ana':
            declare_ana = 1
            del args[0]
        elif args[0] == '--define':
            define = 1
            del args[0]
        elif args[0] == '--define_ana':
            define_ana = 1
            del args[0]
        elif args[0] == '--undefine':
            undefine = 1
            del args[0]
        elif args[0] == '--check_declarations':
            check_declarations = 1
            del args[0]
        elif args[0] == '--check_declarations_ana':
            check_declarations_ana = 1
            del args[0]
        elif args[0] == '--test_declarations':
            test_declarations = 1
            del args[0]
        elif args[0] == '--test_declarations_ana':
            test_declarations_ana = 1
            del args[0]
        elif args[0] == '--check_definition':
            check_definition = 1
            del args[0]
        elif args[0] == '--check_definition_ana':
            check_definition_ana = 1
            del args[0]
        elif args[0] == '--test_definition':
            test_definition = 1
            del args[0]
        elif args[0] == '--test_definition_ana':
            test_definition_ana = 1
            del args[0]
        elif args[0] == '--add_locations':
            add_locations = 1
            del args[0]
        elif args[0] == '--add_locations_ana':
            add_locations_ana = 1
            del args[0]
        elif args[0] == '--check_locations':
            check_locations = 1
            del args[0]
        elif args[0] == '--check_locations_ana':
            check_locations_ana = 1
            del args[0]
        elif args[0] == '--upload':
            upload = 1
            del args[0]
        elif args[0] == '--upload_ana':
            upload_ana = 1
            del args[0]
        elif args[0] == '--check_tape':
            check_tape = 1
            del args[0]
        elif args[0] == '--check_tape_ana':
            check_tape_ana = 1
            del args[0]
        elif args[0] == '--clean_locations':
            clean_locations = 1
            del args[0]
        elif args[0] == '--clean_locations_ana':
            clean_locations_ana = 1
            del args[0]
        elif args[0] == '--remove_locations':
            remove_locations = 1
            del args[0]
        elif args[0] == '--remove_locations_ana':
            remove_locations_ana = 1
            del args[0]
        else:
            print('Unknown option %s' % args[0])
            return 1

    # Normalize xml file path.

    xmlfile = normxmlpath(xmlfile)

    # Make sure xmlfile was specified.

    if xmlfile == '':
        print('No xml file specified.  Type "project.py -h" for help.')
        return 1

    # Make sure that no more than one action was specified (except clean, shorten, and info
    # options).

    num_action = submit + check + checkana + fetchlog + merge + mergehist + mergentuple + audit + stage_status + makeup + define + define_ana + undefine + declare + declare_ana
    if num_action > 1:
        print('More than one action was specified.')
        return 1

    # Extract all project definitions.

    projects = get_projects(xmlfile, check=(not nocheck))

    # Get the selected project element.

    for stagename in stagenames:
        project = select_project(projects, projectname, stagename)
        if project != None:
            if projectname == '':
                projectname = project.name
        else:
            raise RuntimeError('No project selected.\n')

    # Do clean action now.  Cleaning can be combined with submission.

    if clean:
        for stagename in stagenames:
            docleanx(projects, projectname, stagename, clean_descendants = True)

    # Do clean_one action now.  Cleaning can be combined with submission.

    if clean_one:
        for stagename in stagenames:
            docleanx(projects, projectname, stagename, clean_descendants = False)

    # Do stage_status now.

    if stage_status:
        dostatus(projects)
        return 0

    # Get the current stage definition, and pubsify it if necessary.
    # Also process any command line stage configuration overrides.

    stages = {}
    for stagename in stagenames:
        stage = project.get_stage(stagename)
        stages[stagename] = stage

        # Command line configuration overrides handled here.

        if lines != '':
            stage.lines = lines
        if site != '':
            stage.site = site
        if cpu != 0:
            stage.cpu = cpu
        if disk != '':
            stage.disk = disk
        if memory != 0:
            stage.memory = memory
        if inputdef != '':
            stage.inputdef = inputdef
            stage.inputfile = ''
            stage.inputlist = ''
        if recur != 0:
            stage.recur = recur

        # Pubs mode overrides handled here.

        if pubs:
            stage.pubsify_input(pubs_run, pubs_subruns, pubs_version)
            stage.pubsify_output(pubs_run, pubs_subruns, pubs_version)

        # Make recursive dataset definition here, if necessary.

        if stage.recur and stage.inputdef != '' and stage.basedef != '':

            # First check if stage.inptudef already exists.

            import_samweb()
            def_exists = False
            try:
                desc = samweb.descDefinition(defname=stage.inputdef)
                def_exists = True
            except samweb_cli.exceptions.DefinitionNotFound:
                pass

            if not def_exists:

                # Recurcive definition doesn't exist, so create it.

                project_utilities.test_kca()

                # Start sam dimension with the base dataset.

                dim = ''

                # Add minus clause.

                project_wildcard = '%s_%%' % samweb.makeProjectName(stage.inputdef).rsplit('_',1)[0]
                if stage.recurtype == 'snapshot':
                    dim = 'defname: %s minus snapshot_for_project_name %s' % \
                        (stage.basedef, project_wildcard)
                elif stage.recurtype == 'consumed':
                    dim = 'defname: %s minus (project_name %s and consumed_status consumed)' % \
                        (stage.basedef, project_wildcard)

                elif stage.recurtype == 'child':

                    # In case of multiple data strams, generate one clause for each
                    # data stream.

                    nstream = 1
                    if stage.data_stream != None and len(stage.data_stream) > 0:
                        nstream = len(stage.data_stream)

                    dim = ''
                    for istream in range(nstream):
                        idim = project_utilities.dimensions_datastream(project, stage, 
                                                                       ana=False, index=istream)
                        if idim.find('anylocation') > 0:
                            idim = idim.replace('anylocation', 'physical')
                        else:
                            idim += ' with availability physical'

                        if len(dim) > 0:
                            dim += ' or '
                        dim += '(defname: %s minus isparentof:( %s ) )' % (stage.basedef, idim)

                    if stage.activebase != '':
                        activedef = '%s_active' % stage.activebase
                        waitdef = '%s_wait' % stage.activebase
                        dim += ' minus defname: %s' % activedef
                        dim += ' minus defname: %s' % waitdef
                        project_utilities.makeDummyDef(activedef)
                        project_utilities.makeDummyDef(waitdef)

                elif stage.recurtype == 'anachild':

                    # In case of multiple data strams, generate one clause for each
                    # data stream.

                    nstream = 1
                    if stage.ana_data_stream != None and len(stage.ana_data_stream) > 0:
                        nstream = len(stage.ana_data_stream)

                    dim = ''
                    for istream in range(nstream):
                        idim = project_utilities.dimensions_datastream(project, stage, 
                                                                       ana=True, index=istream)
                        if idim.find('anylocation') > 0:
                            idim = idim.replace('anylocation', 'physical')
                        else:
                            idim += ' with availability physical'

                        if len(dim) > 0:
                            dim += ' or '
                        dim += '(defname: %s minus isparentof:( %s ) )' % (stage.basedef, idim)

                    if stage.activebase != '':
                        activedef = '%s_active' % stage.activebase
                        waitdef = '%s_wait' % stage.activebase
                        dim += ' minus defname: %s' % activedef
                        dim += ' minus defname: %s' % waitdef
                        project_utilities.makeDummyDef(activedef)
                        project_utilities.makeDummyDef(waitdef)

                elif stage.recurtype != '' and stage.recurtype != 'none':
                    raise RuntimeError('Unknown recursive type %s.' % stage.recurtype)

                # Add "with limit" clause.

                if stage.recurlimit != 0:
                    dim += ' with limit %d' % stage.recurlimit

                # Create definition.

                print('Creating recursive dataset definition %s' % stage.inputdef)
                project_utilities.test_kca()
                samweb.createDefinition(defname=stage.inputdef, dims=dim)


    # Do dump stage action now.

    if dump_stage:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            print(stage)

    # Do dump project action now.

    if dump_project:
        print(project)

    # Do outdir action now.

    if print_outdir:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            print(stage.outdir)

    # Do logdir action now.

    if print_logdir:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            print(stage.logdir)

    # Do logdir action now.

    if print_workdir:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            print(stage.workdir)

    # Do bookdir action now.

    if print_bookdir:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            print(stage.bookdir)

    # Do defname action now.

    if defname:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.defname != '':
                print(stage.defname)

    # Do input_names action now.

    if do_input_files:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            input_files = get_input_files(stage)
            for input_file in input_files:
                print(input_file)

    # Do check_submit action now.

    if do_check_submit:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            stage.checksubmit()

    # Do check_input action now.

    if do_check_input:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            stage.checkinput(checkdef=True)

    # Do shorten action now.

    if shorten:
        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            doshorten(stage)

    # Do actions.

    rc = 0

    if submit or makeup:

        # Submit jobs.

        for stagename in stagenames:
            print('Stage %s:' % stagename)

            if project_utilities.check_running(xmlfile, stagename):
                print('Skipping job submission because similar job submission process is running.')
            else:
                stage = stages[stagename]
                dosubmit(project, stage, makeup, stage.recur, dryrun)

    if check or checkana:

        # Check results from specified project stage.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            docheck(project, stage, checkana or stage.ana, stage.validate_on_worker)

    if fetchlog:

        # Fetch logfiles.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            rc += dofetchlog(project, stage)

    if mergehist or mergentuple or merge:

        # Make merged histogram or ntuple files using proper hadd option.
        # Makes a merged root file called anahist.root in the project output directory

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            domerge(stage, mergehist, mergentuple)

    if audit:

        # Sam audit.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            doaudit(stage)

    if check_definition or define:

        # Make sam dataset definition.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.ana:
                if stage.ana_defname == '':
                    print('No sam analysis dataset definition name specified for this stage.')
                    return 1
                dim = project_utilities.dimensions_datastream(project, stage, ana=True)
                docheck_definition(stage.ana_defname, dim, define)
            else:
                if stage.defname == '':
                    print('No sam dataset definition name specified for this stage.')
                    return 1
                dim = project_utilities.dimensions_datastream(project, stage, ana=False)
                docheck_definition(stage.defname, dim, define)

    if check_definition_ana or define_ana:

        # Make sam dataset definition for analysis files.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.ana_defname == '':
                print('No sam analysis dataset definition name specified for this stage.')
                return 1
            dim = project_utilities.dimensions_datastream(project, stage, ana=True)
            docheck_definition(stage.ana_defname, dim, define_ana)

    if test_definition:

        # Print summary of files returned by dataset definition.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.ana:
                if stage.ana_defname == '':
                    print('No sam dataset definition name specified for this stage.')
                    return 1
                rc += dotest_definition(stage.ana_defname)
            else:
                if stage.defname == '':
                    print('No sam dataset definition name specified for this stage.')
                    return 1
                rc += dotest_definition(stage.defname)

    if test_definition_ana:

        # Print summary of files returned by analysis dataset definition.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.ana_defname == '':
                print('No sam dataset definition name specified for this stage.')
                return 1
            rc += dotest_definition(stage.ana_defname)

    if undefine:

        # Delete sam dataset definition.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            if stage.defname == '':
                print('No sam dataset definition name specified for this stage.')
                return 1
            rc += doundefine(stage.defname)

    if check_declarations or declare:

        # Check sam declarations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            docheck_declarations(stage.bookdir, stage.outdir, declare, ana=stage.ana)

    if check_declarations_ana or declare_ana:

        # Check sam analysis declarations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            docheck_declarations(stage.bookdir, stage.outdir, declare_ana, ana=True)

    if test_declarations:

        # Print summary of declared files.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=stage.ana)
            rc += dotest_declarations(dim)

    if test_declarations_ana:

        # Print summary of declared files.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=True)
            rc += dotest_declarations(dim)

    if check_locations or add_locations or clean_locations or remove_locations or upload:

        # Check sam disk locations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=stage.ana)
            docheck_locations(dim, stage.outdir,
                              add_locations, clean_locations, remove_locations,
                              upload)

    if check_locations_ana or add_locations_ana or clean_locations_ana or \
       remove_locations_ana or upload_ana:

        # Check sam disk locations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=True)
            docheck_locations(dim, stage.outdir,
                              add_locations_ana, clean_locations_ana, remove_locations_ana,
                              upload_ana)

    if check_tape:

        # Check sam tape locations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=stage.ana)
            docheck_tape(dim)

    if check_tape_ana:

        # Check analysis file sam tape locations.

        for stagename in stagenames:
            print('Stage %s:' % stagename)
            stage = stages[stagename]
            dim = project_utilities.dimensions_datastream(project, stage, ana=True)
            docheck_tape(dim)

    # Done.

    return rc

# Open and truncate a file for writing using larbatch_posix.open.

def safeopen(destination):
    if larbatch_posix.exists(destination):
        larbatch_posix.remove(destination)
    file = larbatch_posix.open(destination, 'w')
    return file

# Invoke main program.

#Utility funciton to scan a file and return its contents as a list
def scan_file(fileName):
    #openable = 1
    returnArray = []
    try:
        #print 'Reading %s' % fileName
        fileList = project_utilities.saferead(fileName)
    #if we can't find missing_files the check will not work
    except:
        #print 'Cannot open file: %s' % fileName
        return [ -1 ]

    if len(fileList) > 0:
        for line in fileList:
            returnArray.append(line.strip())

    else:
        #print '%s exists, but is empty' % fileName

        return [ -1 ]

    return returnArray

if __name__ == '__main__':
    sys.exit(main(sys.argv))

'''inputlist = []
                inp = open(stage.inputlist,"r")
                for line in inp:
                    columns = line.split("/")
                    columns = [col.strip() for col in columns]
                    inputlist.append(columns[8])
                inp.close()'''
