
MODULE GE
    use :: ISO_C_BINDING

    interface GE_Protect
        MODULE PROCEDURE GE_Protect_Fortran_var_REAL_K4
        MODULE PROCEDURE GE_Protect_Fortran_var_REAL_K8
    end interface ! GE_Protect

    interface GE_Snapshot
        MODULE PROCEDURE GE_Snapshot_Fortran_REAL_K4
        MODULE PROCEDURE GE_Snapshot_Fortran_REAL_K8
    end interface ! GE_Snapshot


    CONTAINS

    subroutine GE_Init
        CALL GE_Init_c()
    end subroutine GE_Init

    subroutine GE_Increase_Counter
        CALL GE_Increase_Counter_c()
    end subroutine GE_Increase_Counter

    subroutine GE_PrintResult
        CALL GE_PrintResult_c()
    end subroutine GE_PrintResult

    subroutine GE_Finalize
        CALL GE_Finalize_c()
    end subroutine GE_Finalize

    subroutine GE_Protect_Fortran_var_REAL_K4(VARNAME, VAR, THRESHOLD, WINDOW, METHOD, USE_CHG_RATIO, GRANULARITY)
        implicit none
        character(len=*) :: VARNAME
        real(kind=4), DIMENSION(:) :: VAR
        real(kind=4) :: THRESHOLD
        integer(kind=4) :: R1
        integer(kind=4) :: WINDOW
        integer(kind=4) :: METHOD
        integer(kind=4) :: USE_CHG_RATIO
        integer(kind=4) :: GRANULARITY
        integer(kind=4) :: VARNAME_LEN
        VARNAME_LEN = LEN_TRIM(VARNAME)
        R1 = size(VAR, 1)
        CALL GE_Protect_1d_Float(VARNAME, VARNAME_LEN, R1, THRESHOLD, WINDOW, METHOD, USE_CHG_RATIO, GRANULARITY)
    end subroutine

    subroutine GE_Protect_Fortran_var_REAL_K8(VARNAME, VAR, THRESHOLD, WINDOW, METHOD, USE_CHG_RATIO, GRANULARITY)
        implicit none
        character(len=*) :: VARNAME
        real(kind=8), DIMENSION(:) :: VAR
        real(kind=8) :: THRESHOLD
        integer(kind=4) :: WINDOW
        integer(kind=4) :: METHOD
        integer(kind=4) :: USE_CHG_RATIO
        integer(kind=4) :: GRANULARITY
        integer(kind=4) :: R1
        integer(kind=4) :: VARNAME_LEN
        R1 = size(VAR, 1)
        VARNAME_LEN = LEN_TRIM(VARNAME)
        CALL GE_Protect_1d_Double(VARNAME, VARNAME_LEN, R1, THRESHOLD, WINDOW, METHOD, USE_CHG_RATIO, GRANULARITY)
    end subroutine

    subroutine GE_Snapshot_Fortran_REAL_K4(VARNAME, DATA)
        implicit none
        character(len=*) :: VARNAME
        real(kind=4), DIMENSION(:) :: DATA
        integer(kind=4) :: VARNAME_LEN
        VARNAME_LEN = LEN_TRIM(VARNAME)
        CALL GE_Snapshot_1d_1var_Float(VARNAME, VARNAME_LEN, DATA);
    end subroutine GE_Snapshot_Fortran_REAL_K4

    subroutine GE_Snapshot_Fortran_REAL_K8(VARNAME, DATA)
        implicit none
        character(len=*) :: VARNAME
        real(kind=8), DIMENSION(:) :: DATA
        integer(kind=4) :: VARNAME_LEN
        VARNAME_LEN = LEN_TRIM(VARNAME)
        CALL GE_Snapshot_1d_1var_Double(VARNAME, VARNAME_LEN, DATA);
    end subroutine GE_Snapshot_Fortran_REAL_K8
    
END MODULE GE
