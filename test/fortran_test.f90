program test_f

!INCLUDE 'common.h'

integer :: curr_step, steps, i, res, buf_size
real*8 :: max, threshold, ratio
real*8, dimension(10) :: temperature
integer :: THRESHOLD_METHOD = 1, FAULT = 1
external ge_detect_init
external ge_detect_verify
integer  ge_detect_verify

external ge_detect_finish

threshold = 0.03
steps = 10
buf_size = 10

do i = 1, 10
    temperature(i) = 12.0
end do

call ge_detect_init(THRESHOLD_METHOD, 0, threshold)

do curr_step = 1, steps
    print *, "in step : ", curr_step
    max = 0.0
    ! simulate the calculation for temerature calculation
    do i = 1, 10
        ratio = rand()
	print *, "ratio: ",  ratio
	if ( max < ratio) then
	    max = ratio
	end if
	ratio = ratio + 1.0
	temperature(i) = temperature(i) * ratio
	print *, "temperature: ",  temperature(i)
    end do

    res = -1
    res = ge_detect_verify(temperature, buf_size)
    print *, 'res = ', res
    	if (res == FAULT) then
        print *, "a fault detected with max change ratio : ", max
    end if
end do

call ge_detect_finish()

end program test_f
