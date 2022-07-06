; ModuleID = 'src//kiss_fftr.cpp'
source_filename = "src//kiss_fftr.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }
%struct.kiss_fftr_state = type { %struct.kiss_fft_state*, %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx* }
%struct.kiss_fft_state = type { i32, i32, [64 x i32], [1 x %struct.kiss_fft_cpx] }
%struct.kiss_fft_cpx = type { float, float }

@stderr = external dso_local local_unnamed_addr global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [37 x i8] c"Real FFT optimization must be even.\0A\00", align 1
@.str.1 = private unnamed_addr constant [38 x i8] c"kiss fft usage error: improper alloc\0A\00", align 1

; Function Attrs: uwtable
define dso_local %struct.kiss_fftr_state* @kiss_fftr_alloc(i32 %nfft, i32 %inverse_fft, i8* %mem, i64* %lenmem) local_unnamed_addr #0 {
entry:
  %subsize = alloca i64, align 8
  %0 = bitcast i64* %subsize to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* nonnull %0) #7
  %and = and i32 %nfft, 1
  %tobool = icmp eq i32 %and, 0
  br i1 %tobool, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  %1 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !tbaa !2
  %2 = tail call i64 @fwrite(i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str, i64 0, i64 0), i64 36, i64 1, %struct._IO_FILE* %1) #8
  br label %cleanup

if.end:                                           ; preds = %entry
  %shr = ashr i32 %nfft, 1
  %call1 = call %struct.kiss_fft_state* @kiss_fft_alloc(i32 %shr, i32 %inverse_fft, i8* null, i64* nonnull %subsize)
  %3 = load i64, i64* %subsize, align 8, !tbaa !6
  %mul = mul nsw i32 %shr, 3
  %div = sdiv i32 %mul, 2
  %conv = sext i32 %div to i64
  %mul2 = shl nsw i64 %conv, 3
  %add = add nsw i64 %mul2, 24
  %add3 = add i64 %add, %3
  %cmp = icmp eq i64* %lenmem, null
  br i1 %cmp, label %if.then4, label %if.else

if.then4:                                         ; preds = %if.end
  %call5 = call noalias i8* @malloc(i64 %add3) #7
  br label %if.end9

if.else:                                          ; preds = %if.end
  %4 = load i64, i64* %lenmem, align 8, !tbaa !6
  %cmp6 = icmp ult i64 %4, %add3
  store i64 %add3, i64* %lenmem, align 8, !tbaa !6
  br i1 %cmp6, label %cleanup, label %if.end9

if.end9:                                          ; preds = %if.else, %if.then4
  %st.1.in = phi i8* [ %call5, %if.then4 ], [ %mem, %if.else ]
  %st.1 = bitcast i8* %st.1.in to %struct.kiss_fftr_state*
  %tobool10 = icmp eq i8* %st.1.in, null
  br i1 %tobool10, label %cleanup, label %if.end12

if.end12:                                         ; preds = %if.end9
  %add.ptr = getelementptr inbounds i8, i8* %st.1.in, i64 24
  %5 = bitcast i8* %st.1.in to i8**
  store i8* %add.ptr, i8** %5, align 8, !tbaa !8
  %add.ptr14 = getelementptr inbounds i8, i8* %add.ptr, i64 %3
  %6 = bitcast i8* %add.ptr14 to %struct.kiss_fft_cpx*
  %tmpbuf = getelementptr inbounds i8, i8* %st.1.in, i64 8
  %7 = bitcast i8* %tmpbuf to i8**
  store i8* %add.ptr14, i8** %7, align 8, !tbaa !10
  %idx.ext = sext i32 %shr to i64
  %add.ptr16 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %6, i64 %idx.ext
  %super_twiddles = getelementptr inbounds i8, i8* %st.1.in, i64 16
  %8 = bitcast i8* %super_twiddles to %struct.kiss_fft_cpx**
  store %struct.kiss_fft_cpx* %add.ptr16, %struct.kiss_fft_cpx** %8, align 8, !tbaa !11
  %call18 = call %struct.kiss_fft_state* @kiss_fft_alloc(i32 %shr, i32 %inverse_fft, i8* nonnull %add.ptr, i64* nonnull %subsize)
  %cmp2079 = icmp sgt i32 %nfft, 3
  br i1 %cmp2079, label %for.body.lr.ph, label %cleanup

for.body.lr.ph:                                   ; preds = %if.end12
  %div19 = sdiv i32 %shr, 2
  %conv23 = sitofp i32 %shr to double
  %9 = fdiv fast double 1.000000e+00, %conv23
  %tobool27 = icmp eq i32 %inverse_fft, 0
  %10 = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %8, align 8, !tbaa !11
  %11 = sext i32 %div19 to i64
  br i1 %tobool27, label %for.body.us, label %for.body

for.body.us:                                      ; preds = %for.body.lr.ph, %for.body.us
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.body.us ], [ 0, %for.body.lr.ph ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %12 = trunc i64 %indvars.iv.next to i32
  %conv22.us = sitofp i32 %12 to double
  %13 = fmul fast double %conv22.us, 0xC00921FB54442D18
  %14 = fmul fast double %13, %9
  %mul26.us = fadd fast double %14, 0xBFF921FB54442D18
  %15 = call fast double @llvm.cos.f64(double %mul26.us)
  %conv31.us = fptrunc double %15 to float
  %r.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %10, i64 %indvars.iv, i32 0
  store float %conv31.us, float* %r.us, align 4, !tbaa !12
  %16 = call fast double @llvm.sin.f64(double %mul26.us)
  %conv35.us = fptrunc double %16 to float
  %i39.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %10, i64 %indvars.iv, i32 1
  store float %conv35.us, float* %i39.us, align 4, !tbaa !15
  %cmp20.us = icmp slt i64 %indvars.iv.next, %11
  br i1 %cmp20.us, label %for.body.us, label %cleanup

for.body:                                         ; preds = %for.body.lr.ph, %for.body
  %indvars.iv82 = phi i64 [ %indvars.iv.next83, %for.body ], [ 0, %for.body.lr.ph ]
  %indvars.iv.next83 = add nuw nsw i64 %indvars.iv82, 1
  %17 = trunc i64 %indvars.iv.next83 to i32
  %conv22 = sitofp i32 %17 to double
  %18 = fmul fast double %conv22, 0xC00921FB54442D18
  %19 = fmul fast double %18, %9
  %mul26 = fadd fast double %19, 0xBFF921FB54442D18
  %20 = call fast double @llvm.cos.f64(double %mul26)
  %conv31 = fptrunc double %20 to float
  %r = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %10, i64 %indvars.iv82, i32 0
  store float %conv31, float* %r, align 4, !tbaa !12
  %21 = call fast double @llvm.sin.f64(double %mul26)
  %22 = fptrunc double %21 to float
  %conv35 = fsub fast float -0.000000e+00, %22
  %i39 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %10, i64 %indvars.iv82, i32 1
  store float %conv35, float* %i39, align 4, !tbaa !15
  %cmp20 = icmp slt i64 %indvars.iv.next83, %11
  br i1 %cmp20, label %for.body, label %cleanup

cleanup:                                          ; preds = %for.body, %for.body.us, %if.end12, %if.else, %if.end9, %if.then
  %retval.0 = phi %struct.kiss_fftr_state* [ null, %if.then ], [ null, %if.end9 ], [ null, %if.else ], [ %st.1, %if.end12 ], [ %st.1, %for.body.us ], [ %st.1, %for.body ]
  call void @llvm.lifetime.end.p0i8(i64 8, i8* nonnull %0) #7
  ret %struct.kiss_fftr_state* %retval.0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

declare dso_local %struct.kiss_fft_state* @kiss_fft_alloc(i32, i32, i8*, i64*) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare dso_local noalias i8* @malloc(i64) local_unnamed_addr #3

; Function Attrs: nounwind readnone speculatable
declare double @llvm.cos.f64(double) #4

; Function Attrs: nounwind readnone speculatable
declare double @llvm.sin.f64(double) #4

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: uwtable
define dso_local void @kiss_fftr(%struct.kiss_fftr_state* nocapture readonly %st, float* %timedata, %struct.kiss_fft_cpx* nocapture %freqdata) local_unnamed_addr #0 {
entry:
  %substate = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 0
  %0 = load %struct.kiss_fft_state*, %struct.kiss_fft_state** %substate, align 8, !tbaa !8
  %inverse = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %0, i64 0, i32 1
  %1 = load i32, i32* %inverse, align 4, !tbaa !16
  %tobool = icmp eq i32 %1, 0
  br i1 %tobool, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  %2 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !tbaa !2
  %3 = tail call i64 @fwrite(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.1, i64 0, i64 0), i64 37, i64 1, %struct._IO_FILE* %2) #8
  tail call void @exit(i32 1) #9
  unreachable

if.end:                                           ; preds = %entry
  %nfft = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %0, i64 0, i32 0
  %4 = load i32, i32* %nfft, align 4, !tbaa !19
  %5 = bitcast float* %timedata to %struct.kiss_fft_cpx*
  %tmpbuf = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 1
  %6 = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %tmpbuf, align 8, !tbaa !10
  tail call void @kiss_fft(%struct.kiss_fft_state* %0, %struct.kiss_fft_cpx* %5, %struct.kiss_fft_cpx* %6)
  %7 = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %tmpbuf, align 8, !tbaa !10
  %r = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 0, i32 0
  %8 = load float, float* %r, align 4, !tbaa !12
  %i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 0, i32 1
  %9 = load float, float* %i, align 4, !tbaa !15
  %add = fadd fast float %9, %8
  %r11 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 0, i32 0
  store float %add, float* %r11, align 4, !tbaa !12
  %sub = fsub fast float %8, %9
  %idxprom = sext i32 %4 to i64
  %r15 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %idxprom, i32 0
  store float %sub, float* %r15, align 4, !tbaa !12
  %i17 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 0, i32 1
  store float 0.000000e+00, float* %i17, align 4, !tbaa !15
  %i20 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %idxprom, i32 1
  store float 0.000000e+00, float* %i20, align 4, !tbaa !15
  %cmp190 = icmp slt i32 %4, 2
  br i1 %cmp190, label %for.end, label %for.body.lr.ph

for.body.lr.ph:                                   ; preds = %if.end
  %div194 = lshr i32 %4, 1
  %super_twiddles = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 2
  %10 = add nuw nsw i32 %div194, 1
  %wide.trip.count = zext i32 %10 to i64
  %.pre = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %super_twiddles, align 8, !tbaa !11
  br label %for.body

for.body:                                         ; preds = %for.body, %for.body.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.body ], [ 1, %for.body.lr.ph ]
  %fpk.sroa.0.0..sroa_idx = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %indvars.iv, i32 0
  %fpk.sroa.0.0.copyload = load float, float* %fpk.sroa.0.0..sroa_idx, align 4
  %fpk.sroa.5.0..sroa_idx144 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %indvars.iv, i32 1
  %fpk.sroa.5.0.copyload = load float, float* %fpk.sroa.5.0..sroa_idx144, align 4
  %11 = sub nsw i64 %idxprom, %indvars.iv
  %r28 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %11, i32 0
  %12 = load float, float* %r28, align 4, !tbaa !12
  %i34 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %11, i32 1
  %13 = load float, float* %i34, align 4, !tbaa !15
  %add39 = fadd fast float %12, %fpk.sroa.0.0.copyload
  %add43 = fsub fast float %fpk.sroa.5.0.copyload, %13
  %sub48 = fsub fast float %fpk.sroa.0.0.copyload, %12
  %14 = fadd fast float %13, %fpk.sroa.5.0.copyload
  %15 = add nsw i64 %indvars.iv, -1
  %r61 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %.pre, i64 %15, i32 0
  %16 = load float, float* %r61, align 4, !tbaa !12
  %mul = fmul fast float %16, %sub48
  %i67 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %.pre, i64 %15, i32 1
  %17 = load float, float* %i67, align 4, !tbaa !15
  %mul68 = fmul fast float %17, %14
  %sub69 = fsub fast float %mul, %mul68
  %mul77 = fmul fast float %17, %sub48
  %mul84 = fmul fast float %16, %14
  %add85 = fadd fast float %mul77, %mul84
  %add91 = fadd fast float %sub69, %add39
  %conv93 = fmul fast float %add91, 5.000000e-01
  %r96 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %indvars.iv, i32 0
  store float %conv93, float* %r96, align 4, !tbaa !12
  %add99 = fadd fast float %add85, %add43
  %conv102 = fmul fast float %add99, 5.000000e-01
  %i105 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %indvars.iv, i32 1
  store float %conv102, float* %i105, align 4, !tbaa !15
  %sub108 = fsub fast float %add39, %sub69
  %conv111 = fmul fast float %sub108, 5.000000e-01
  %r115 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %11, i32 0
  store float %conv111, float* %r115, align 4, !tbaa !12
  %sub118 = fsub fast float %add85, %add43
  %conv121 = fmul fast float %sub118, 5.000000e-01
  %i125 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %11, i32 1
  store float %conv121, float* %i125, align 4, !tbaa !15
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %if.end
  ret void
}

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32) local_unnamed_addr #5

declare dso_local void @kiss_fft(%struct.kiss_fft_state*, %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx*) local_unnamed_addr #2

; Function Attrs: uwtable
define dso_local void @kiss_fftri(%struct.kiss_fftr_state* nocapture readonly %st, %struct.kiss_fft_cpx* nocapture readonly %freqdata, float* %timedata) local_unnamed_addr #0 {
entry:
  %substate = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 0
  %0 = load %struct.kiss_fft_state*, %struct.kiss_fft_state** %substate, align 8, !tbaa !8
  %inverse = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %0, i64 0, i32 1
  %1 = load i32, i32* %inverse, align 4, !tbaa !16
  %cmp = icmp eq i32 %1, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %2 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !tbaa !2
  %3 = tail call i64 @fwrite(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.1, i64 0, i64 0), i64 37, i64 1, %struct._IO_FILE* %2) #8
  tail call void @exit(i32 1) #9
  unreachable

if.end:                                           ; preds = %entry
  %nfft = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %0, i64 0, i32 0
  %4 = load i32, i32* %nfft, align 4, !tbaa !19
  %r = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 0, i32 0
  %5 = load float, float* %r, align 4, !tbaa !12
  %idxprom = sext i32 %4 to i64
  %r3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %idxprom, i32 0
  %6 = load float, float* %r3, align 4, !tbaa !12
  %add = fadd fast float %6, %5
  %tmpbuf = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 1
  %7 = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %tmpbuf, align 8, !tbaa !10
  %r5 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 0, i32 0
  store float %add, float* %r5, align 4, !tbaa !12
  %8 = load float, float* %r, align 4, !tbaa !12
  %9 = load float, float* %r3, align 4, !tbaa !12
  %sub = fsub fast float %8, %9
  %i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 0, i32 1
  store float %sub, float* %i, align 4, !tbaa !15
  %cmp13186 = icmp slt i32 %4, 2
  br i1 %cmp13186, label %for.end, label %for.body.lr.ph

for.body.lr.ph:                                   ; preds = %if.end
  %div190 = lshr i32 %4, 1
  %super_twiddles = getelementptr inbounds %struct.kiss_fftr_state, %struct.kiss_fftr_state* %st, i64 0, i32 2
  %10 = add nuw nsw i32 %div190, 1
  %wide.trip.count = zext i32 %10 to i64
  %.pre = load %struct.kiss_fft_cpx*, %struct.kiss_fft_cpx** %super_twiddles, align 8, !tbaa !11
  br label %for.body

for.body:                                         ; preds = %for.body, %for.body.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.body ], [ 1, %for.body.lr.ph ]
  %fk.sroa.0.0..sroa_idx = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %indvars.iv, i32 0
  %fk.sroa.0.0.copyload = load float, float* %fk.sroa.0.0..sroa_idx, align 4
  %fk.sroa.5.0..sroa_idx141 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %indvars.iv, i32 1
  %fk.sroa.5.0.copyload = load float, float* %fk.sroa.5.0..sroa_idx141, align 4
  %11 = sub nsw i64 %idxprom, %indvars.iv
  %r19 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %11, i32 0
  %12 = load float, float* %r19, align 4, !tbaa !12
  %i24 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %freqdata, i64 %11, i32 1
  %13 = load float, float* %i24, align 4, !tbaa !15
  %add29 = fadd fast float %12, %fk.sroa.0.0.copyload
  %add33 = fsub fast float %fk.sroa.5.0.copyload, %13
  %sub38 = fsub fast float %fk.sroa.0.0.copyload, %12
  %14 = fadd fast float %13, %fk.sroa.5.0.copyload
  %15 = add nsw i64 %indvars.iv, -1
  %r51 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %.pre, i64 %15, i32 0
  %16 = load float, float* %r51, align 4, !tbaa !12
  %mul = fmul fast float %16, %sub38
  %i57 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %.pre, i64 %15, i32 1
  %17 = load float, float* %i57, align 4, !tbaa !15
  %mul58 = fmul fast float %17, %14
  %sub59 = fsub fast float %mul, %mul58
  %mul67 = fmul fast float %17, %sub38
  %mul74 = fmul fast float %16, %14
  %add75 = fadd fast float %mul67, %mul74
  %add82 = fadd fast float %sub59, %add29
  %r86 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %indvars.iv, i32 0
  store float %add82, float* %r86, align 4, !tbaa !12
  %add89 = fadd fast float %add75, %add33
  %i93 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %indvars.iv, i32 1
  store float %add89, float* %i93, align 4, !tbaa !15
  %sub99 = fsub fast float %add29, %sub59
  %r104 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %11, i32 0
  store float %sub99, float* %r104, align 4, !tbaa !12
  %i112 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %7, i64 %11, i32 1
  %18 = fsub fast float %add75, %add33
  store float %18, float* %i112, align 4, !tbaa !15
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %if.end
  %19 = bitcast float* %timedata to %struct.kiss_fft_cpx*
  tail call void @kiss_fft(%struct.kiss_fft_state* %0, %struct.kiss_fft_cpx* %7, %struct.kiss_fft_cpx* %19)
  ret void
}

; Function Attrs: nofree nounwind
declare i64 @fwrite(i8* nocapture, i64, i64, %struct._IO_FILE* nocapture) local_unnamed_addr #6

attributes #0 = { uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #3 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #4 = { nounwind readnone speculatable }
attributes #5 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #6 = { nofree nounwind }
attributes #7 = { nounwind }
attributes #8 = { cold }
attributes #9 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://gitlab.engr.illinois.edu/llvm/hpvm-release.git f83c355d11be7d011508763fedc67e76d592a636)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"long", !4, i64 0}
!8 = !{!9, !3, i64 0}
!9 = !{!"_ZTS15kiss_fftr_state", !3, i64 0, !3, i64 8, !3, i64 16}
!10 = !{!9, !3, i64 8}
!11 = !{!9, !3, i64 16}
!12 = !{!13, !14, i64 0}
!13 = !{!"_ZTS12kiss_fft_cpx", !14, i64 0, !14, i64 4}
!14 = !{!"float", !4, i64 0}
!15 = !{!13, !14, i64 4}
!16 = !{!17, !18, i64 4}
!17 = !{!"_ZTS14kiss_fft_state", !18, i64 0, !18, i64 4, !4, i64 8, !4, i64 264}
!18 = !{!"int", !4, i64 0}
!19 = !{!17, !18, i64 0}
